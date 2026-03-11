#pragma once


#include <QMainWindow>
#include <QScopedPointer>
//#include "core/dsp/IqReceiver.h"
// #include "io/audio/IqAudioInputDevice.h"
#include "core/radio/receiver/IqReceiver.h"
#include <QLineSeries>
#include <QAreaSeries>
#include <QGraphicsLineItem>
#include <QValueAxis>

#include "io/audio/drivers/RtAudio/RtAudioOutputDriver.h"
#include "core/radio/Radio.h"
#include "core/config-settings/config/RadioConfig.h"
#include "common/QtPanadapter.h"
#include "faces/FaceBase.h"
// #include "settings/RadioSettingsEventPublisher.h"

class QVBoxLayout;
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
  bool eventFilter(QObject *watched, QEvent *event) override;

public slots:
  // void on_actionConfigure_triggered();
  void on_actionBand_triggered();
  // void on_actionMode_triggered();
  // void on_actionLevels_triggered();

protected:
  // void handleReceiverIqEvent(const vsdrcomplex* data, uint32_t length, uint32_t sampleRate);
  // void handleReceiverAudioEvent(const vsdrreal* data, uint32_t length);
  // void handleTransmitterIqEvent(const vsdrcomplex* data, uint32_t length, uint32_t sampleRate);
  // void handleTransmitterAudioEvent(const vsdrreal* data, uint32_t length);
  void handleRadioSettingsEvent(const RadioSettings& radioSettings, uint64_t sequence);

  void closeActiveToolbarPopups();



//private slots:
//    void toggleMode();
//    void toggleSuspend();
//    void deviceChanged(int index);
//    void sliderChanged(int value);

private:
  void initializeWindow();
  void initializeAudio();

  void initialiseRadio();

  void setFaceByName(const std::string& faceName);

  void addModeButton();
  QMenu* createModeMenu(const Mode& currentMode);
  void updateModeButton(const Mode& mode);
  void updateBandButton(const Band& band);
  // void updateModeMenu(const Mode& mode);
  void addLevelsButton();
  void addConfigButton();
  void addBandButton();

private:
  RadioConfig& m_radioConfig;
  Ui::MainWindow *ui;

  std::unique_ptr<FaceBase> m_pFace;
  QVBoxLayout* m_pFaceLayout;

  Radio* m_pRadio;

  uint32_t m_reportedIqSampleRate;
  RadioSettings m_radioSettingsCopy;

  QToolButton* m_modeButton;
  QToolButton* m_bandButton{};
  // QtTimeSeriesChart* m_pTimeSeriesChart;
  // QtPanadapter* m_pPanadapter;
};

