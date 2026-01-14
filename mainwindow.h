#pragma once


#include <QMainWindow>
#include <QScopedPointer>
//#include "dsp/IqReceiver.h"
// #include "io/audio/IqAudioInputDevice.h"
#include "radio/receiver/IqReceiver.h"
#include <QLineSeries>
#include <QAreaSeries>
#include <QGraphicsLineItem>
#include <QValueAxis>

#include "io/audio/drivers/RtAudio/RtAudioOutputDriver.h"
#include "radio/Radio.h"
#include "config/RadioConfig.h"
#include "ui/qt/QtPanadapter.h"
// #include "settings/RadioSettingsEventPublisher.h"

class QtTimeSeriesChart;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QToolButton;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(RadioConfig& radioConfig, QWidget *parent = nullptr);
  ~MainWindow() override;

  void customEvent(QEvent* event) override;

public slots:
  // void on_actionConfigure_triggered();
  void on_actionBand_triggered();
  // void on_actionMode_triggered();
  // void on_actionLevels_triggered();

protected:
  void handleReceiverIqEvent(const vsdrcomplex* data, uint32_t length, uint32_t sampleRate);
  void handleReceiverAudioEvent(const vsdrreal* data, uint32_t length);
  void handleTransmitterIqEvent(const vsdrcomplex* data, uint32_t length, uint32_t sampleRate);
  void handleTransmitterAudioEvent(const vsdrreal* data, uint32_t length);
  void handleRadioSettingsEvent(const RadioSettings& radioSettings, const BandSettings& bandSettings);

//private slots:
//    void toggleMode();
//    void toggleSuspend();
//    void deviceChanged(int index);
//    void sliderChanged(int value);

private:
  void initializeWindow();
  void initializeAudio();

  void initialiseRadio();

  void addModeButton();
  QMenu* createModeMenu(const Mode& currentMode);
  void updateModeButton(const Mode& mode);
  // void updateModeMenu(const Mode& mode);
  void addLevelsButton();
  void addConfigButton();
  void addBandButton();

private:
  RadioConfig& m_radioConfig;
  Ui::MainWindow *ui;

  Radio* m_pRadio;

  uint32_t m_reportedIqSampleRate;
  RadioSettings m_radioSettingsCopy;
  BandSettings m_bandSettingsCopy;


  QToolButton* m_modeButton;
  QToolButton* m_bandButton{};
  QtTimeSeriesChart* m_pTimeSeriesChart;
  QtPanadapter* m_pPanadapter;
};

