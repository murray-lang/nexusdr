#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaDevices>
#include <QScopedPointer>
#include <QAudioSource>
#include <QAudioSink>
//#include "dsp/IqReceiver.h"
// #include "io/audio/IqAudioInputDevice.h"
#include "radio/receiver/IqReceiver.h"
#include <QLineSeries>
#include <QAreaSeries>

#include "io/audio/device/AudioOutputDevice.h"
#include "radio/config/RadioConfig.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(RadioConfig& radioConfig, QWidget *parent = nullptr);
  ~MainWindow() override;

  void customEvent(QEvent* event) override;

protected:
  void handleReceiverIqEvent(const vsdrcomplex* data, uint32_t length);
  void handleReceiverAudioEvent(const vsdrreal* data, uint32_t length);
  static void powerSpectrum(const std::vector<sdrcomplex>& timeSeries, uint32_t timeSeriesLength, vsdrreal& spectrumOut);

  static void calcSpectrumSeries(const vsdrreal* spectrumData, QLineSeries& spectrumSeries, bool shuffle = true);
  static void calcSpectrumSeries(const vsdrcomplex* spectrumData, QLineSeries& spectrumSeries, bool shuffle = true);

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
  RadioConfig m_radioConfig;
  Ui::MainWindow *ui;

//    bool m_pullMode = true;
//    IqReceiver m_iqProcessor;
  IqReceiver* m_pIqReceiver;
  QLineSeries m_spectrumLineSeries;
  QAreaSeries m_spectrumAreaSeries;

  QLineSeries m_timeseriesLineSeries;

  uint32_t m_panadapterXmin;
  uint32_t m_panadapterXmax;
  uint32_t m_timeSeriesXmin;
  uint32_t m_timeSeriesXmax;
};
#endif // MAINWINDOW_H
