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
// #include "settings/RadioSettingsEventPublisher.h"

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
  static void powerSpectrum(const std::vector<sdrcomplex>& timeSeries, uint32_t timeSeriesLength, vsdrreal& spectrumOut);

  void replaceSpectrumSeries(
    const vsdrreal* spectrumData,
    QLineSeries& spectrumSeries,
    uint32_t sampleRate,
    uint32_t centreFrequency,
    bool shuffle = true
  );
  void replaceSpectrumSeries(
    const vsdrcomplex* spectrumData,
    QLineSeries& spectrumSeries,
    uint32_t sampleRate,
    uint32_t centreFrequency,
    bool shuffle = true
  );

//private slots:
//    void toggleMode();
//    void toggleSuspend();
//    void deviceChanged(int index);
//    void sliderChanged(int value);

private:
  void initializeWindow();
  //void initializeAudio(const QAudioDevice &deviceInfo);
  void initializeAudio();
  void configurePanadapter();
  void setPanadapterX(uint32_t xMin, uint32_t xMax);
  void configureTimeseriesChart();
  void setTimeSeriesX(uint32_t xMin, uint32_t xMax);

  void addPassbandOverlay(QChart *chart, int32_t loCut, int32_t hiCut);
  void updatePassbandOverlay(QChart *chart, int32_t loCut, int32_t hiCut);

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

//    bool m_pullMode = true;
  Radio* m_pRadio;
  QLineSeries m_spectrumLineSeries;
  QAreaSeries m_spectrumAreaSeries;

  QLineSeries m_timeseriesLineSeries;

  uint32_t m_reportedIqSampleRate;
  uint32_t m_panadapterXmin;
  uint32_t m_panadapterXmax;
  uint32_t m_timeSeriesXmin;
  uint32_t m_timeSeriesXmax;
  RadioSettings m_radioSettingsCopy;
  BandSettings m_bandSettingsCopy;

  QGraphicsLineItem *m_verticalCursorLine;
  QGraphicsRectItem * m_filterPassbandRect;

  QToolButton* m_modeButton;
  QToolButton* m_bandButton{};
};

