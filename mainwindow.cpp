#include "mainwindow.h"
#include "ui_mainwindow-1024x600.h"
#include <QSlider>
#include <QThreadPool>
#include <QStyle>
#include <QVariant>
#include <QMenu>
#include <QActionGroup>
#include <cmath>
#include "io/control/device/usb/UsbException.h"
#include "io/control/device/FunCubeDongle/FunCubeDongle.h"
#include <volk/volk.h>

#include "radio/receiver/ReceiverAudioEvent.h"
#include "radio/receiver/ReceiverIqEvent.h"
#include "radio/transmitter/TransmitterAudioEvent.h"
#include "radio/transmitter/TransmitterIqEvent.h"

#include <QToolButton>
#include "ui/qt/QtChartTheme.h"
#include "settings/Bands.h"
#include "settings/RadioSettingsEvent.h"
#include "ui/qt/QtBandDialog.h"
#include "ui/qt/QtTimeSeriesChart.h"

#define FFT_SIZE 2048
#define SAMPLE_RATE 192000

constexpr const char * toolbarPopupPropertyName = "isToolbarPopup";

MainWindow::MainWindow(RadioConfig& radioConfig, QWidget *parent)
  : QMainWindow(parent)
  , m_radioConfig(radioConfig)
  , m_pRadio(nullptr)
  , ui(new Ui::MainWindow)
  , m_reportedIqSampleRate(0)
  ,m_modeButton(nullptr)
  ,m_bandButton(nullptr)
  ,m_pPanadapter(nullptr)
  ,m_pTimeSeriesChart(nullptr)
{

  initialiseRadio();
  initializeWindow();

  initializeAudio();

  m_pPanadapter = new QtPanadapter(this, "panadapterView", "chartTheme");
  m_pPanadapter->initialise();

  m_pTimeSeriesChart = new QtTimeSeriesChart(this, "timeseriesView", "chartTheme");
  m_pTimeSeriesChart->initialise();

  if (this->centralWidget()) {
    // This filter has been added to help manage popup behaviour associated with the toolbar buttons.
    // this->centralWidget()->installEventFilter(this);
  }
}

MainWindow::~MainWindow()
{
  delete m_pRadio;
  delete ui;
  delete m_pPanadapter;
  delete m_pTimeSeriesChart;
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
  RxPipelineSettings* rxPipelineSettings = m_bandSettingsCopy.getFocusRxPipelineSettings();
  if (rxPipelineSettings != nullptr) {
    const RfSettings& rfSettings = rxPipelineSettings->getRfSettings();
    uint32_t centreFrequency = rfSettings.getFrequency();
    uint32_t xMin = centreFrequency - (sampleRate / 2);
    uint32_t xMax = centreFrequency + (sampleRate / 2);

    m_pPanadapter->setSeriesXMinMax(xMin, xMax);
    m_pPanadapter->plot(data, length, sampleRate, centreFrequency, true);
  }
}

void
MainWindow::handleTransmitterIqEvent(const vsdrcomplex* data, uint32_t length, uint32_t sampleRate)
{
  m_reportedIqSampleRate = sampleRate;
  TxPipelineSettings* txPipelineSettings = m_bandSettingsCopy.getTxPipelineSettings();
  if (txPipelineSettings != nullptr) {
    const RfSettings& rfSettings = txPipelineSettings->getRfSettings();
    uint32_t centreFrequency = rfSettings.getFrequency();
    uint32_t xMin = centreFrequency - (sampleRate / 2);
    uint32_t xMax = centreFrequency + (sampleRate / 2);

    m_pPanadapter->setSeriesXMinMax(xMin, xMax);
    m_pPanadapter->plot(data, length, sampleRate, centreFrequency, true);
  }

  m_pTimeSeriesChart->plot(*data, length);
}

void
MainWindow::handleReceiverAudioEvent(const vsdrreal* data, uint32_t length)
{
  m_pTimeSeriesChart->plot(*data, length);
}

void
MainWindow::handleTransmitterAudioEvent(const vsdrreal* data, uint32_t length)
{
  m_pTimeSeriesChart->plot(*data, length);
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

    if (m_reportedIqSampleRate > 0) {
      uint32_t xMin = centreFrequency - (m_reportedIqSampleRate / 2);
      uint32_t xMax = centreFrequency + (m_reportedIqSampleRate / 2);
      m_pPanadapter->setSeriesXMinMax(xMin, xMax);
    }
    const Mode& mode = rxPipelineSettings->getMode();
    m_pPanadapter->updateCursorPosition(frequencyAtOffset, mode.getLoCut(), mode.getHiCut());
    updateModeButton(mode);
  }
  m_radioSettingsCopy.clearChanged();
}

void
MainWindow::on_actionBand_triggered()
{
  if (m_bandButton != nullptr) {
    // QWidget* centralWidget = this->centralWidget();
    QWidget* existing = findChild<QWidget*>("bandPanel");
    if (existing) {
      existing->deleteLater();
      return;
    }
    auto* panel = new QtBandDialog(m_pRadio, this);
    panel->setAttribute(Qt::WA_DeleteOnClose);
    panel->setObjectName("bandPanel");
    panel->setProperty(toolbarPopupPropertyName, true);

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

void
MainWindow::closeActiveToolbarPopups()
{
  const auto panels = this->centralWidget()->findChildren<QWidget*>();
  for (auto* panel : panels) {
    if (panel->property(toolbarPopupPropertyName).toBool()) {
      panel->deleteLater();
    }
  }
}

bool
MainWindow::eventFilter(QObject *watched, QEvent *event)
{
  // if (watched == this->centralWidget() && event->type() == QEvent::MouseButtonPress) {
  //   auto* mouseEvent = static_cast<QMouseEvent*>(event);
  //
  //   const auto panels = this->centralWidget()->findChildren<QWidget*>();
  //   for (auto* panel : panels) {
  //     if (panel->property(toolbarPopupPropertyName).toBool() && panel->isVisible()) {
  //       // If the click is outside this panel, close it
  //       if (!panel->geometry().contains(mouseEvent->pos())) {
  //         panel->deleteLater();
  //         return true; // Consume the event so it doesn't click the UI behind
  //       }
  //     }
  //   }
  // }
  return QMainWindow::eventFilter(watched, event);
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

  auto* chartTheme = new QtChartTheme(this);
  chartTheme->setObjectName("chartTheme");
  chartTheme->setFixedSize(0, 0); // User can't see it, but Style Engine will style it

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

