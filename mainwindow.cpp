#include "mainwindow.h"
#include "ui_mainwindow-1024x600.h"
#include <QSlider>
#include <QPushButton>
#include <QLineSeries>
#include <QValueAxis>
#include <QLogValueAxis>
#include <QThreadPool>
#include <QStyle>
#include <QColor>
#include <QVariant>
#include <QMenu>
#include <QActionGroup>
#include <QDialog>
#include <QVBoxLayout>
#include "io/audio/drivers/RtAudio/RtAudioInputDriver.h"
#include <cmath>
#include "io/control/device/usb/UsbException.h"
#include "io/control/device/FunCubeDongle/FunCubeDongle.h"
#include <io/control/ControlException.h>
#include <config/AudioConfig.h>
#include <volk/volk.h>

#include "radio/receiver/ReceiverAudioEvent.h"
#include "radio/receiver/ReceiverIqEvent.h"
#include "radio/transmitter/TransmitterAudioEvent.h"
#include "radio/transmitter/TransmitterIqEvent.h"

#include <QToolButton>
#include "ui/qt/ChartTheme.h"
#include "settings/Bands.h"
#include "settings/RadioSettingsEvent.h"
#include "ui/qt/BandDialog.h"

#define FFT_SIZE 2048
#define SAMPLE_RATE 192000

MainWindow::MainWindow(RadioConfig& radioConfig, QWidget *parent)
  : QMainWindow(parent)
  , m_radioConfig(radioConfig)
  , m_pRadio(nullptr)
  , m_spectrumLineSeries()
  , m_spectrumAreaSeries()
  , m_timeseriesLineSeries()
  , ui(new Ui::MainWindow)
  , m_reportedIqSampleRate(0)
  , m_panadapterXmin(0)
  , m_panadapterXmax(FFT_SIZE)
  , m_timeSeriesXmin(0)
  , m_timeSeriesXmax(FFT_SIZE),
  m_verticalCursorLine(new QGraphicsLineItem()),
  m_filterPassbandRect(nullptr),
  m_modeButton(nullptr)
{

  initialiseRadio();
  initializeWindow();

  initializeAudio();

  ui->panadapterView->setRenderHint(QPainter::Antialiasing);
  ui->timeseriesView->setRenderHint(QPainter::Antialiasing);

  configurePanadapter();
  configureTimeseriesChart();
}

MainWindow::~MainWindow()
{
  delete m_pRadio;
  delete ui;
}

void
MainWindow::configurePanadapter()
{

  QChart* pChart = ui->panadapterView->chart();

  m_spectrumAreaSeries.setUpperSeries(&m_spectrumLineSeries);

  auto* theme = findChild<ChartTheme*>("panadapterTheme");
  theme->ensurePolished();

  QString backgroundColorStr = theme->property("backgroundColor").toString();
  auto backgroundColor = QColor(backgroundColorStr);
  pChart->setBackgroundBrush(backgroundColor);

  QString plotAreaColorStr = theme->property("plotAreaColor").toString();
  pChart->setPlotAreaBackgroundBrush(QBrush(QColor(plotAreaColorStr)));
  pChart->setPlotAreaBackgroundVisible(true);

  QString seriesLineColorStr = theme->property("seriesLineColor").toString();
  auto seriesLineColor = QColor(seriesLineColorStr);
  QPen pen(seriesLineColor); //0x0080ff
  pen.setWidth(0);
  // m_spectrumLineSeries.setPen(pen);
  m_spectrumAreaSeries.setPen(pen);

  QString seriesAreaColorStr = theme->property("seriesAreaColor").toString();
  auto seriesAreaColor = QColor(seriesAreaColorStr);
  QBrush seriesBrush(seriesAreaColor);
  m_spectrumAreaSeries.setBrush(seriesBrush);

  QString gridColorStr = theme->property("gridColor").toString();
  QPen gridPen(gridColorStr);


  pChart->addSeries(&m_spectrumAreaSeries);
  // pChart->addSeries(&m_spectrumLineSeries);
  // pChart->setTitle("Panadapter");

  QString textColorStr = theme->property("textColor").toString();
  QColor textColor(textColorStr);
  pChart->setTitleBrush(QBrush(textColor));

  pChart->createDefaultAxes();

  for (auto* axis : pChart->axes()) {
    axis->setGridLinePen(gridPen);
    axis->setLinePen(gridPen);
    axis->setLabelsColor(textColor);
  }

  // QLogValueAxis *yAxis = new QLogValueAxis();
  // yAxis->setBase(10.0);
  // yAxis->setRange(-140, 0);
  // pChart->setAxisY(yAxis, &m_spectrumLineSeries);

  QList<QAbstractAxis*> xAxes = pChart->axes(Qt::Horizontal);
  if (!xAxes.isEmpty()) {
    auto *xAxis = qobject_cast<QValueAxis*>(xAxes.first());
    xAxis->setRange(m_panadapterXmin, m_panadapterXmax);
    xAxis->setLabelFormat(QString("%i"));
  }
  pChart->axes(Qt::Vertical).first()->setRange(-110, -50);

  pChart->legend()->hide();

  QString cursorLineColorStr = theme->property("cursorLineColor").toString();
  m_verticalCursorLine->setPen(QPen(QColor(cursorLineColorStr), 1.5, Qt::SolidLine));
  pChart->scene()->addItem(m_verticalCursorLine);
}

void
MainWindow::setPanadapterX(uint32_t xMin, uint32_t xMax)
{
    m_panadapterXmin = xMin;
    m_panadapterXmax = xMax;
    QChart* pChart = ui->panadapterView->chart();
    pChart->axes(Qt::Horizontal).first()->setRange(xMin, xMax);
}

void
MainWindow::configureTimeseriesChart()
{
    QChart* pChart = ui->timeseriesView->chart();

    QPen pen(QRgb(0x0080ff));
    pen.setWidth(0);
    m_timeseriesLineSeries.setPen(pen);

  pChart->addSeries(&m_timeseriesLineSeries);
  // pChart->setTitle("Timeseries");
  pChart->createDefaultAxes();
  for (auto* axis : pChart->axes()) {
    axis->setLabelsVisible(false);
  }
  pChart->axes(Qt::Horizontal).first()->setRange(m_timeSeriesXmin / 100.0, m_timeSeriesXmax/100.0);

//  pChart->axes(Qt::Vertical).first()->setRange(2038, 2058);
  pChart->axes(Qt::Vertical).first()->setRange(-0.05, 0.05);

//  pChart->axes(Qt::Vertical).first()->setRange(-0.0000001, 0.0000003);

  pChart->legend()->hide();
}

void
MainWindow::setTimeSeriesX(uint32_t xMin, uint32_t xMax)
{
    m_timeSeriesXmin = xMin;
    m_timeSeriesXmax = xMax;
    QChart* pChart = ui->timeseriesView->chart();
    pChart->axes(Qt::Horizontal).first()->setRange(m_timeSeriesXmin, m_timeSeriesXmax);
}


void
MainWindow::customEvent(QEvent* event)
{
  if (event->type() == ReceiverIqEvent::RxIqEvent) {
    auto* iqEvent = dynamic_cast<ReceiverIqEvent*>(event);
    handleReceiverIqEvent(iqEvent->buffer.get(), iqEvent->dataLength, iqEvent->sampleRate);
  } else if (event->type() == ReceiverAudioEvent::RxAudioEvent) {
    auto* audioEvent = dynamic_cast<ReceiverAudioEvent*>(event);
    handleReceiverAudioEvent(audioEvent->buffer.get(), audioEvent->dataLength);
  } else if (event->type() == RadioSettingsEvent::RadioSettingsEventType) {
    auto* radioSettingsEvent = dynamic_cast<RadioSettingsEvent*>(event);
    handleRadioSettingsEvent(radioSettingsEvent->getRadioSettings(), radioSettingsEvent->getBandSettings());
  } else if (event->type() == TransmitterIqEvent::TxIqEvent) {
    auto* iqEvent = dynamic_cast<TransmitterIqEvent*>(event);
    handleTransmitterIqEvent(iqEvent->buffer.get(), iqEvent->dataLength, iqEvent->sampleRate);
  } else if (event->type() == TransmitterAudioEvent::TxAudioEvent) {
    auto* audioEvent = dynamic_cast<TransmitterAudioEvent*>(event);
    handleTransmitterAudioEvent(audioEvent->buffer.get(), audioEvent->dataLength);
  }
}

void
MainWindow::handleReceiverIqEvent(const vsdrcomplex* data, uint32_t length, uint32_t sampleRate)
{
  m_reportedIqSampleRate = sampleRate;
  vsdrreal spectrum(length);
  powerSpectrum(*data, length, spectrum);
  // if (spectrum.size() != m_panadapterXmax) {
  //   setPanadapterX(0, spectrum.size());
  // }
  RxPipelineSettings* rxPipelineSettings = m_bandSettingsCopy.getFocusRxPipelineSettings();
  if (rxPipelineSettings != nullptr) {
    const RfSettings& rfSettings = rxPipelineSettings->getRfSettings();
    uint32_t centreFrequency = rfSettings.getFrequency();
    uint32_t xMin = centreFrequency - (sampleRate / 2);
    uint32_t xMax = centreFrequency + (sampleRate / 2);
    if (m_panadapterXmin != xMin || m_panadapterXmax != xMax) {
      setPanadapterX(xMin, xMax);
    }
    replaceSpectrumSeries(&spectrum, m_spectrumLineSeries, sampleRate, centreFrequency, true);
  }
}

void
MainWindow::handleTransmitterIqEvent(const vsdrcomplex* data, uint32_t length, uint32_t sampleRate)
{
  m_reportedIqSampleRate = sampleRate;
  vsdrreal spectrum(length);
  powerSpectrum(*data, length, spectrum);
  // if (spectrum.size() != m_panadapterXmax) {
  //   setPanadapterX(0, spectrum.size());
  // }
  TxPipelineSettings* txPipelineSettings = m_bandSettingsCopy.getTxPipelineSettings();
  if (txPipelineSettings != nullptr) {
    const RfSettings& rfSettings = txPipelineSettings->getRfSettings();
    uint32_t centreFrequency = rfSettings.getFrequency();
    uint32_t xMin = centreFrequency - (sampleRate / 2);
    uint32_t xMax = centreFrequency + (sampleRate / 2);
    if (m_panadapterXmin != xMin || m_panadapterXmax != xMax) {
      setPanadapterX(xMin, xMax);
    }
    replaceSpectrumSeries(&spectrum, m_spectrumLineSeries, sampleRate, centreFrequency, true);
  }

  setTimeSeriesX(0, length);
  //  setTimeSeriesX(0, 48);

  //}
  QList<QPointF> timeseriesPoints;
  uint32_t plotX = 0;
  for (uint32_t i = 0; i < length; i++) {
    timeseriesPoints.append(QPointF(plotX++, data->at(i).imag()));
  }

  m_timeseriesLineSeries.replace(timeseriesPoints);
}

void
MainWindow::handleReceiverAudioEvent(const vsdrreal* data, uint32_t length)
{
  setTimeSeriesX(0, length);
  //  setTimeSeriesX(0, 48);

  //}
  QList<QPointF> timeseriesPoints;
  uint32_t plotX = 0;
  for (uint32_t i = 0; i < length; i++) {
    timeseriesPoints.append(QPointF(plotX++, data->at(i)));
  }

  m_timeseriesLineSeries.replace(timeseriesPoints);
}

void
MainWindow::handleTransmitterAudioEvent(const vsdrreal* data, uint32_t length)
{
  setTimeSeriesX(0, length);
  //  setTimeSeriesX(0, 48);

  //}
  QList<QPointF> timeseriesPoints;
  uint32_t plotX = 0;
  for (uint32_t i = 0; i < length; i++) {
    timeseriesPoints.append(QPointF(plotX++, data->at(i)));
  }

  m_timeseriesLineSeries.replace(timeseriesPoints);
}

void
MainWindow::handleRadioSettingsEvent(const RadioSettings& radioSettings, const BandSettings& bandSettings)
{
  m_radioSettingsCopy = radioSettings;
  m_bandSettingsCopy = bandSettings;
  RxPipelineSettings* rxPipelineSettings = m_bandSettingsCopy.getFocusRxPipelineSettings();
  if (rxPipelineSettings == nullptr) {
    return;
  }
  RfSettings& rfSettings = rxPipelineSettings->getRfSettings();
  bool frequencyChanged = (rfSettings.hasSettingChanged(RfSettings::Features::FREQUENCY)) != 0;
  bool offsetChanged = (rfSettings.hasSettingChanged(RfSettings::Features::OFFSET)) != 0;
  bool modeChanged = (rxPipelineSettings->hasSettingChanged(PipelineSettings::Features::MODE)) != 0;

  if (frequencyChanged || offsetChanged || modeChanged) {
    auto centreFrequency = static_cast<int32_t>(rfSettings.getFrequency());
    int32_t offset = rfSettings.getOffset();
    int32_t frequencyAtOffset = centreFrequency + offset;

    ui->centreFrequencyLcd->display(centreFrequency);
    ui->cursorFrequencyLcd->display(frequencyAtOffset);

    QChart *chart = ui->panadapterView->chart();
    auto *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
    double yMin = axisY->min();
    double yMax = axisY->max();

    // qDebug() << "Centre frequency changed to" << centreFrequency << "Offset" << offset << "Frequency at offset" << frequencyAtOffset;
    if (m_reportedIqSampleRate > 0) {
      uint32_t xMin = centreFrequency - (m_reportedIqSampleRate / 2);
      uint32_t xMax = centreFrequency + (m_reportedIqSampleRate / 2);
      if (m_panadapterXmin != xMin || m_panadapterXmax != xMax) {
        setPanadapterX(xMin, xMax);
      }
    } 

    QPointF p1 = chart->mapToPosition(QPointF(frequencyAtOffset, yMin));
    QPointF p2 = chart->mapToPosition(QPointF(frequencyAtOffset, yMax));
    // qDebug() << "Centre frequency" << centreFrequency << "Offset" << offset << "Frequency at offset" << frequencyAtOffset << "Mapped to" << p1 << p2; 
    m_verticalCursorLine->setLine(QLineF(p1, p2));
    m_verticalCursorLine->show();
  
    const Mode& mode = rxPipelineSettings->getMode();
    int32_t loCutWrtFreq = frequencyAtOffset + mode.getLoCut();
    int32_t hioCutWrtFreq = frequencyAtOffset + mode.getHiCut();
    updatePassbandOverlay(chart, loCutWrtFreq, hioCutWrtFreq);
    updateModeButton(mode);
  }
  m_radioSettingsCopy.clearChanged();
}

void
MainWindow::addPassbandOverlay(QChart *chart, int32_t loCut, int32_t hiCut)
{
  auto* theme = findChild<ChartTheme*>("panadapterTheme");
  QString cursorAreaColorStr = theme->property("cursorAreaColor").toString();

  m_filterPassbandRect = new QGraphicsRectItem(loCut, 0, hiCut - loCut, 100);
  m_filterPassbandRect->setBrush(QColor(cursorAreaColorStr));
  // m_filterPassbandRect->setBrush(QColor(100, 50, 200, 80)); // Semi-transparent
  m_filterPassbandRect->setPen(Qt::NoPen);
  chart->scene()->addItem(m_filterPassbandRect);

}

void
MainWindow::updatePassbandOverlay(QChart *chart, int32_t loCut, int32_t hiCut)
{
  if (m_filterPassbandRect == nullptr) {
    addPassbandOverlay(chart, loCut, hiCut);
  }
  QRectF plotArea = chart->plotArea();
  double plotLoX = chart->mapToPosition(QPointF(loCut, 0)).x();
  double plotHiX = chart->mapToPosition(QPointF(hiCut, 0)).x();
  double width = plotHiX - plotLoX;
  QRectF r(plotLoX, plotArea.top(), width, plotArea.height());
  m_filterPassbandRect->setRect(r);

}

void
MainWindow::powerSpectrum(const vsdrcomplex& timeSeries, uint32_t timeSeriesLength, vsdrreal& spectrumOut)
{
  vsdrcomplex windowed(timeSeriesLength);
  for (uint32_t i = 0; i < timeSeriesLength; i++)
  {
    windowed.at(i) = timeSeries.at(i) * static_cast<sdrreal>(hanning(i, timeSeriesLength));
  }
  pocketfft::shape_t pocketfft_shape{timeSeriesLength};
  std::vector<sdrcomplex> fftOut(timeSeriesLength);

  spectrumOut.resize(timeSeriesLength);

  pocketfft::stride_t pocketfft_stride{sizeof(sdrcomplex)};
  pocketfft::shape_t pocketfft_axes{0};

  pocketfft::c2c(
      pocketfft_shape,
      pocketfft_stride,
      pocketfft_stride,
      pocketfft_axes,
      pocketfft::FORWARD,
      windowed.data(),
      fftOut.data(),
      static_cast<sdrreal>(1.0)
  );

  volk_32fc_s32f_x2_power_spectral_density_32f(
    spectrumOut.data(),
    fftOut.data(),
    static_cast<float>(timeSeriesLength), 1.0,
    timeSeriesLength
  );
}

void
MainWindow::replaceSpectrumSeries(
  const vsdrreal * spectrumData,
  QLineSeries& spectrumSeries,
  uint32_t sampleRate,
  uint32_t centreFrequency,
  bool shuffle
)
{
  qreal plotX = centreFrequency - (sampleRate / 2);
  qreal binWidth = static_cast<qreal>(sampleRate) / static_cast<qreal>(spectrumData->size());

  QList<QPointF> spectrumPoints;
  size_t fftSize = spectrumData->size();
  if (shuffle)
  {
    for (size_t bin = fftSize/2; bin < fftSize; bin++) {
      spectrumPoints.append(QPointF(plotX, spectrumData->at(bin)));
      plotX += binWidth;
    }
    for (size_t bin = 0; bin < fftSize/2 -1; bin++) {
      spectrumPoints.append(QPointF(plotX, spectrumData->at(bin)));
      plotX += binWidth;
    }
  } else
  {
    for (size_t bin = 0; bin < fftSize; bin++) {
      spectrumPoints.append(QPointF(plotX, spectrumData->at(bin)));
      plotX += binWidth;
    }
  }
  spectrumSeries.replace(spectrumPoints);
}

void
MainWindow::replaceSpectrumSeries(
  const std::vector<sdrcomplex> * spectrumData,
  QLineSeries& spectrumSeries,
  uint32_t sampleRate,
  uint32_t centreFrequency,
  bool shuffle
)
{
  qreal plotX = centreFrequency - (sampleRate / 2);
  qreal binWidth = sampleRate / spectrumData->size();

  QList<QPointF> spectrumPoints;
  size_t fftSize = spectrumData->size();
  if (shuffle)
  {
    for (size_t bin = fftSize/2; bin < fftSize; bin++) {
      spectrumPoints.append(QPointF(plotX, std::abs(spectrumData->at(bin))));
      plotX += binWidth;
    }
    for (size_t bin = 0; bin < fftSize/2 -1; bin++) {
      spectrumPoints.append(QPointF(plotX, std::abs(spectrumData->at(bin))));
      plotX += binWidth;
    }
  } else
  {
    for (size_t bin = 0; bin < fftSize; bin++) {
      spectrumPoints.append(QPointF(plotX, std::abs(spectrumData->at(bin))));
      plotX += binWidth;
    }
  }
  spectrumSeries.replace(spectrumPoints);
}

// void
// MainWindow::on_actionConfigure_triggered()
// {
//   qDebug() << "on_actionConfigure_triggered()";
// }

void
MainWindow::on_actionBand_triggered()
{
  if (m_bandButton != nullptr) {
    // QWidget* centralWidget = this->centralWidget();
    QWidget* existing = findChild<QWidget*>("bandPanel");
    if (existing) {
      existing->close();
      return;
    }
    auto* panel = new BandDialog(m_pRadio, this);
    panel->setAttribute(Qt::WA_DeleteOnClose);
    panel->setObjectName("bandPanel");

    // IMPORTANT: Ensure it has no window flags that would make it a separate window
    panel->setWindowFlags(Qt::Widget);

    // We must manually call adjustSize because it's not in a layout
    panel->adjustSize();

    QPoint buttonPos = m_bandButton->mapTo(this, QPoint(0, 0));

    int x = buttonPos.x();
    int y = buttonPos.y() - panel->height();

    panel->move(x, y);
    panel->show();
    panel->raise();
  }
}

// void
// MainWindow::on_actionMode_triggered()
// {
//   qDebug() << "on_actionMode_triggered()";
// }

// void
// MainWindow::on_actionLevels_triggered()
// {
//   qDebug() << "on_actionLevels_triggered()";
// }

void MainWindow::initializeWindow()
{
  ui->setupUi(this);

  auto* panadapterTheme = new ChartTheme(this);
  panadapterTheme->setObjectName("panadapterTheme");
  panadapterTheme->setFixedSize(0, 0); // User can't see it, but Style Engine will style it

  // qDebug() << "Current Icon Theme:" << QIcon::themeName();
  // qDebug() << "Icon Search Paths:" << QIcon::themeSearchPaths();

  addConfigButton();
  addBandButton();

  QWidget* spacer1 = new QWidget();
  spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  ui->toolBar->addWidget(spacer1);

  addModeButton();
  addLevelsButton();

    //m_volumeSlider = new QSlider(Qt::Horizontal, this);
    ui->volumeSlider->setRange(0, 100);
    ui->volumeSlider->setValue(100);
    //connect(ui->volumeSlider, &QSlider::valueChanged, this, &MainWindow::sliderChanged);
//    layout->addWidget(m_volumeSlider);

//    m_modeButton = new QPushButton(this);
//    connect(ui->modeButton, &QPushButton::clicked, this, &MainWindow::toggleMode);
//    layout->addWidget(m_modeButton);

//    m_suspendResumeButton = new QPushButton(this);
//    connect(ui->suspendButton, &QPushButton::clicked, this, &MainWindow::toggleSuspend);
//    layout->addWidget(m_suspendResumeButton);
}

void
MainWindow::addModeButton()
{
  if (m_pRadio == nullptr) {
    throw std::runtime_error("No radio instance");
  }
  RadioSettings& radioSettings = m_pRadio->getRadioSettings();
  const std::string selectedBandName = radioSettings.getBandName();
  BandSettings* bandSettings = m_pRadio->getBandSettings(selectedBandName);
  if (bandSettings == nullptr) {
    throw SettingsException("Band settings not found for selected band");
  }
  RxPipelineSettings* rxPipelineSettings = bandSettings->getFocusRxPipelineSettings();
  if (rxPipelineSettings == nullptr) {
    throw SettingsException("Radio pipeline settings not found for selected band");
  }

  delete m_modeButton;
  m_modeButton = new QToolButton();
  // modeBtn->setDefaultAction(ui->actionMode);
  // tabsBtn->setFixedWidth(100);
  const Mode& mode = rxPipelineSettings->getMode();
  QMenu* modeMenu = createModeMenu(mode);
  updateModeButton(mode);
  modeMenu->setProperty("class", "toolbarMenu mode");
  // modeBtn->setMenu(modeMenu);
  // Set popup mode (InstantPopup makes the button act like a dropdown)
  // modeBtn->setPopupMode(QToolButton::InstantPopup);
  m_modeButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
  m_modeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  // Force the button to draw its background based on the current palette
  m_modeButton->setAutoFillBackground(true);
  m_modeButton->setProperty("class", "toolbarButton toolbarButtonC");

  connect(m_modeButton, &QToolButton::pressed, this, [this, modeMenu]() {
    // Calculate the top-left position of the button in global screen coordinates
    QPoint pos = m_modeButton->mapToGlobal(QPoint(0, 0));

    // Move the point up by the height of the menu
    // hint: sizeHint() is usually accurate for menus before they are shown
    pos.setY(pos.y() - modeMenu->sizeHint().height());

    const Mode& currMode = m_bandSettingsCopy.getFocusRxPipelineMode();
    for (QAction *action : modeMenu->actions()) {
      action->setChecked(currMode.getName() == action->text().toStdString());
    }
    modeMenu->exec(pos);
  });

  ui->toolBar->addWidget(m_modeButton);
}

QMenu*
MainWindow::createModeMenu(const Mode& currentMode)
{
  auto modeMenu = new QMenu(this);

  auto* actionGroup = new QActionGroup(this);
  actionGroup->setExclusive(true); // Only one can be checked at a time

  const std::vector<Mode>& allModes = ModeSettings::getAll();

  SettingUpdatePath settingPath({
    RadioSettings::Features::PIPELINE,
    BandSettings::Features::RX_PIPELINE,
    PipelineSettings::Features::MODE
  });
  for (const auto& mode : allModes) {
    QAction* action = modeMenu->addAction(mode.getName().c_str(), this, [this, mode, settingPath]()
    {
      SettingUpdate setting(settingPath, mode.getType(), SettingUpdate::Meaning::VALUE);
      m_pRadio->applySettingUpdate(setting);
    });
    action->setCheckable(true);
    action->setActionGroup(actionGroup);

    // Highlight the currently active mode
    if (mode.getType() == currentMode.getType()) {
      action->setChecked(true);
    }
  }
  return modeMenu;
}

void
MainWindow::updateModeButton(const Mode& mode)
{
  if (m_modeButton != nullptr) {
    m_modeButton->setText(mode.getName().c_str());
  }
}

// void
// MainWindow::updateModeMenu(const Mode& mode)
// {
//   if (m_modeButton != nullptr) {
//     m_modeButton->setText(mode.getName().c_str());
//   }
// }

void
MainWindow::addLevelsButton()
{
  auto* levelsBtn = new QToolButton();
  // levelsBtn->setDefaultAction(ui->actionLevels);
  // tabsBtn->setFixedWidth(100);
  levelsBtn->setIcon(QIcon(":ui//icons/solid/sliders.svg"));
  levelsBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
  levelsBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  // Force the button to draw its background based on the current palette
  levelsBtn->setAutoFillBackground(true);
  levelsBtn->setProperty("class", "toolbarButton toolbarButtonD");
  ui->toolBar->addWidget(levelsBtn);
}

void
MainWindow::addConfigButton()
{
  auto* configBtn = new QToolButton();
  // configBtn->setDefaultAction(ui->actionConfigure);
  // tabsBtn->setFixedWidth(100);
  configBtn->setIcon(QIcon(":ui//icons/solid/gear.svg"));
  configBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  // Force the button to draw its background based on the current palette
  configBtn->setAutoFillBackground(true);
  configBtn->setProperty("class", "toolbarButton toolbarButtonA");
  ui->toolBar->addWidget(configBtn);
}

void
MainWindow::addBandButton()
{
  m_bandButton = new QToolButton();
  m_bandButton->setDefaultAction(ui->actionBand);
  m_bandButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  m_bandButton->setAutoFillBackground(true);
  m_bandButton->setProperty("class", "toolbarButton toolbarButtonB");
  ui->toolBar->addWidget(m_bandButton);
}

void MainWindow::initializeAudio()
{

// //    QAudioSource* newSource = new QAudioSource(deviceInfo, format);
// //    m_audioInput.reset(newSource);
// //    qreal initialVolume = QAudio::convertVolume(m_audioInput->volume(), QAudio::LinearVolumeScale,
// //                                                QAudio::LogarithmicVolumeScale);
// //    ui->volumeSlider->setValue(qRound(initialVolume * 100));

}

//void
//MainWindow::sliderChanged(int value)
//{
//    qreal linearVolume = QAudio::convertVolume(value / qreal(100), QAudio::LogarithmicVolumeScale,
//                                               QAudio::LinearVolumeScale);
//
//    m_audioInput->setVolume(linearVolume);
//}

void
MainWindow::initialiseRadio()
{
  // const QList<QAudioDevice> inputs = QMediaDevices::audioOutputs();
  // for (const QAudioDevice &device : inputs) {
  //   qDebug() << "Device:" << device.description() << device.id();
  //
  //   qDebug() << "  Minimum sample rate:" << device.minimumSampleRate();
  //   qDebug() << "  Maximum sample rate:" << device.maximumSampleRate();
  //
  //   qDebug() << "  Minimum channel count:" << device.minimumChannelCount();
  //   qDebug() << "  Maximum channel count:" << device.maximumChannelCount();
  //   qDebug() << "  Channel Config:" << device.channelConfiguration();
  //
  //   qDebug() << "  Sample formats supported:";
  //   for (auto format : device.supportedSampleFormats()) {
  //     qDebug() << "    " << format;
  //   }
  // }

  if (m_pRadio != nullptr) {
    m_pRadio->stop();
    delete m_pRadio;
    m_pRadio = nullptr;
  }
  try
  {
    m_pRadio = new Radio(this);
    m_pRadio->configure(&m_radioConfig);
    m_pRadio->start();

    m_pRadio->applyBand("40m");

    // m_radioSettings.bandName = "20m";
    // m_radioSettings.changed = RadioSettings::BAND;
    
    // m_pRadio->applySettings(m_radioSettings);

    RfSettings rfSettings;
    rfSettings.setGain(30.0);
    rfSettings.setGainStep(1.0);
    m_pRadio->applyRfSettings(rfSettings);

    IfSettings ifSettings;
    ifSettings.setGain(0.0);
    ifSettings.setBandwidth(200000);
    m_pRadio->applyIfSettings(ifSettings);
  }
  catch (std::runtime_error& error)
  {
    qDebug() << "Error initialising radio: " << error.what();
  }
}

//#include "moc_mainwindow.cpp"

