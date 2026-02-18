#include "mainwindow.h"
#include "ui_mainwindow-1024x600.h"
#include <QSlider>
#include <QThreadPool>
#include <QStyle>
#include <QVariant>
#include <QMenu>
#include <QActionGroup>
#include <QVBoxLayout>
#include <cmath>
#include "io/control/device/usb/UsbException.h"
#include "io/control/device/FunCubeDongle/FunCubeDongle.h"
#include <volk/volk.h>

#include "radio/receiver/ReceiverAudioEvent.h"
#include "radio/receiver/ReceiverIqEvent.h"
#include "radio/transmitter/TransmitterAudioEvent.h"
#include "radio/transmitter/TransmitterIqEvent.h"

#include <QToolButton>
#include "ui/qt/common/QtChartTheme.h"
#include "config-settings/settings/bands/Bands.h"
#include "config-settings/settings/RadioSettingsEvent.h"
#include "ui/qt/common/QtBandDialog.h"
#include "ui/qt/common/QtTimeSeriesChart.h"
#include "ui/qt/faces/FaceFactory.h"

#define FFT_SIZE 2048
#define SAMPLE_RATE 192000

constexpr const char * toolbarPopupPropertyName = "isToolbarPopup";

MainWindow::MainWindow(RadioConfig& radioConfig, QWidget *parent)
  : QMainWindow(parent)
  , m_radioConfig(radioConfig)
  , m_pRadio(nullptr)
  , ui(new Ui::MainWindow)
  , m_pFaceLayout(new QVBoxLayout)
  , m_reportedIqSampleRate(0)
  ,m_modeButton(nullptr)
  ,m_bandButton(nullptr)
{

  initialiseRadio();
  initializeWindow();

  initializeAudio();

  if (this->centralWidget()) {
    // This filter has been added to help manage popup behaviour associated with the toolbar buttons.
    this->installEventFilter(this);
  }
}

MainWindow::~MainWindow()
{
  delete m_pRadio;
  delete ui;
}

void
MainWindow::customEvent(QEvent* event)
{
  if (m_pFace) {
    if (event->type() == ReceiverIqEvent::RxIqEvent) {
      auto* iqEvent = dynamic_cast<ReceiverIqEvent*>(event);
      m_pFace->handleReceiverIq(&m_radioSettingsCopy,iqEvent->buffer.get(), iqEvent->dataLength, iqEvent->sampleRate);
    } else if (event->type() == ReceiverAudioEvent::RxAudioEvent) {
      auto* audioEvent = dynamic_cast<ReceiverAudioEvent*>(event);
      m_pFace->handleReceiverAudio(audioEvent->buffer.get(), audioEvent->dataLength);
    } else if (event->type() == ReceiverMeterEvent::RxMeterEvent) {
      auto* meterEvent = dynamic_cast<ReceiverMeterEvent*>(event);
      m_pFace->handleReceiverMeter(meterEvent->rssiDbFs(), meterEvent->sampleRate(), meterEvent->agcGainDb());
    } else if (event->type() == RadioSettingsEvent::RadioSettingsEventType) {
      auto* radioSettingsEvent = dynamic_cast<RadioSettingsEvent*>(event);
      handleRadioSettingsEvent(radioSettingsEvent->getRadioSettings(), radioSettingsEvent->getSequence());
    } else if (event->type() == TransmitterIqEvent::TxIqEvent) {
      auto* iqEvent = dynamic_cast<TransmitterIqEvent*>(event);
      m_pFace->handleTransmitterIq(&m_radioSettingsCopy, iqEvent->buffer.get(), iqEvent->dataLength, iqEvent->sampleRate);
    } else if (event->type() == TransmitterAudioEvent::TxAudioEvent) {
      auto* audioEvent = dynamic_cast<TransmitterAudioEvent*>(event);
      m_pFace->handleTransmitterAudio(audioEvent->buffer.get(), audioEvent->dataLength);
    }
  }
}

void
MainWindow::handleRadioSettingsEvent(const RadioSettings& radioSettings, uint64_t sequence)
{
  uint64_t currentSequence = m_pRadio->getUpdateSequence();
  m_radioSettingsCopy = radioSettings;
  auto* bandDialog = findChild<QtBandDialog*>("bandPanel");
  if (bandDialog != nullptr) {
    bandDialog->applySettings(m_radioSettingsCopy);
  }
  BandSettings* bandSettings = RadioSettings::getFocusBandSettings();
  updateBandButton(bandSettings->getBand());

  RxPipelineSettings* rxPipelineSettings = bandSettings->getFocusPipeline();
  if (rxPipelineSettings != nullptr) {
    const Mode& mode = rxPipelineSettings->getMode();
    updateModeButton(mode);
  }
  if (m_pFace) {
    m_pFace->handleRadioSettingsChanged(&m_radioSettingsCopy);
  }
  m_radioSettingsCopy.clearChanged();
  if (sequence == currentSequence) {
    bandSettings->clearChanged();
    // m_radioSettingsCopy.getBandSelector().clearChanged();
  }
}

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
      panel->close();
    }
  }
}

bool
MainWindow::eventFilter(QObject *watched, QEvent *event)
{
  if (watched == this && event->type() == QEvent::MouseButtonPress) {
    auto* mouseEvent = static_cast<QMouseEvent*>(event);

    const auto panels = this->findChildren<QWidget*>();
    for (auto* panel : panels) {
      if (panel->property(toolbarPopupPropertyName).toBool() && panel->isVisible()) {
        // If the click is outside this panel, close it
        if (!panel->geometry().contains(mouseEvent->pos())) {
          panel->close();
          return true; // Consume the event so it doesn't click the UI behind
        }
      }
    }
  }
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

  // auto* chartTheme = new QtChartTheme(this);
  // chartTheme->setObjectName("chartTheme");
  // chartTheme->setFixedSize(0, 0); // User can't see it, but Style Engine will style it

  m_pFaceLayout = new QVBoxLayout(this->centralWidget());
  m_pFaceLayout->setObjectName("faceLayout");
  m_pFaceLayout->setContentsMargins(0, 0, 0, 0);
  m_pFaceLayout->setSpacing(0);

  setFaceByName(m_radioConfig.getUiFaceName());



  // qDebug() << "Current Icon Theme:" << QIcon::themeName();
  // qDebug() << "Icon Search Paths:" << QIcon::themeSearchPaths();

  addConfigButton();
  addBandButton();

  QWidget* spacer1 = new QWidget();
  spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  ui->toolBar->addWidget(spacer1);

  addModeButton();
  addLevelsButton();
}

void
MainWindow::setFaceByName(const std::string& faceName)
{
  const std::string name = faceName.empty() ? FaceFactory::defaultName : faceName;

  auto newFace = FaceFactory::instance().create(name, this->centralWidget());
  if (!newFace) {
    // last-resort: keep current face, or show an error widget
    return;
  }

  if (m_pFace) {
    m_pFaceLayout->removeWidget(m_pFace.get());
    m_pFace.reset();
  }

  m_pFace = std::move(newFace);
  m_pFaceLayout->addWidget(m_pFace.get());
  m_pFace->setRadio(m_pRadio);
  m_pFace->initialise(&m_radioSettingsCopy);
}

void
MainWindow::addModeButton()
{
  if (m_pRadio == nullptr) {
    throw std::runtime_error("No radio instance");
  }
  RadioSettings& radioSettings = m_pRadio->getRadioSettings();
  const BandSettings* bandSettings = m_pRadio->getFocusBandSettings();
  const RxPipelineSettings* rxPipelineSettings = bandSettings->getFocusPipeline();
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

    const Mode* currMode = m_radioSettingsCopy.getFocusRxPipelineMode();
    for (QAction *action : modeMenu->actions()) {
      action->setChecked(currMode->getName() == action->text().toStdString());
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
    RadioSettings::Features::BAND,
    BandSelector::WITH_FOCUS,
    BandSettings::Features::WITH_FOCUS_PIPELINE,
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

void
MainWindow::updateBandButton(const Band& band)
{
  if (m_bandButton != nullptr) {
    m_bandButton->setText(QString::fromStdString(band.getLabel()));
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
    //m_pRadio->split("40m", "20m");
    // m_pRadio->addPipeline();

    RfSettings rfSettings;
    rfSettings.setGain(30.0);
    rfSettings.setGainCoarseStep(1.0);
    rfSettings.setGainFineStep(0.1);
    m_pRadio->applyRfSettings(rfSettings, true);

    // int32_t centreFreqStepCoarse = 50000;
    // m_pRadio->applySetting("pipeline.rx-pipeline.rf.centre-frequency.coarse-step", centreFreqStepCoarse);

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

