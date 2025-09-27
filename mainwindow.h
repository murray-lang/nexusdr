#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>
//#include "dsp/IqReceiver.h"
// #include "io/audio/IqAudioInputDevice.h"
#include "radio/receiver/IqReceiver.h"
#include <QLineSeries>
#include <QAreaSeries>
#include <QGraphicsLineItem>

#include "io/audio/device/AudioOutputDevice.h"
#include "radio/Radio.h"
#include "config/RadioConfig.h"
#include "settings/RadioSettingsEventPublisher.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(RadioConfig& radioConfig, QWidget *parent = nullptr);
  ~MainWindow() override;

  void customEvent(QEvent* event) override;

protected:
  void handleReceiverIqEvent(const vsdrcomplex* data, uint32_t length, uint32_t sampleRate);
  void handleReceiverAudioEvent(const vsdrreal* data, uint32_t length);
  void handleRadioSettingsEvent(const RadioSettings& radioSettings);
  static void powerSpectrum(const std::vector<sdrcomplex>& timeSeries, uint32_t timeSeriesLength, vsdrreal& spectrumOut);

  void replaceSpectrumSeries(
    const vsdrreal* spectrumData,
    QLineSeries& spectrumSeries,
    uint32_t sampleRate,
    bool shuffle = true
  );
  void replaceSpectrumSeries(
    const vsdrcomplex* spectrumData,
    QLineSeries& spectrumSeries,
    uint32_t sampleRate,
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

  void initialiseRadio();
private:
  RadioConfig& m_radioConfig;
  Ui::MainWindow *ui;

//    bool m_pullMode = true;
  Radio* m_pRadio;
  QLineSeries m_spectrumLineSeries;
  QAreaSeries m_spectrumAreaSeries;

  QLineSeries m_timeseriesLineSeries;

  uint32_t m_panadapterXmin;
  uint32_t m_panadapterXmax;
  uint32_t m_timeSeriesXmin;
  uint32_t m_timeSeriesXmax;
  RadioSettings m_radioSettings;

  QGraphicsLineItem *m_verticalCursorLine;
  uint32_t m_currentSampleRate;

};
#endif // MAINWINDOW_H
