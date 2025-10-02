#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSlider>
#include <QPushButton>
#include <QLineSeries>
#include <QValueAxis>
#include <QLogValueAxis>
#include <QThreadPool>
#include "io/audio/device/IqAudioInputDevice.h"
#include <cmath>
#include "io/control/device/usb/UsbException.h"
#include "io/control/device/FunCubeDongle/FunCubeDongle.h"
#include <io/control/ControlException.h>
#include <config/AudioConfig.h>
#include <volk/volk.h>

#include "radio/receiver/ReceiverAudioEvent.h"
#include "radio/receiver/ReceiverIqEvent.h"

#define FFT_SIZE 2048
#define SAMPLE_RATE 192000

MainWindow::MainWindow(RadioConfig& radioConfig, QWidget *parent)
    : QMainWindow(parent)
    , m_radioConfig(radioConfig)
    // , m_pIqReceiver(nullptr)
    , m_pRadio(nullptr)
    , m_spectrumLineSeries()
    , m_spectrumAreaSeries()
    , m_timeseriesLineSeries()
    , ui(new Ui::MainWindow)
    , m_panadapterXmin(0)
    , m_panadapterXmax(FFT_SIZE)
    , m_timeSeriesXmin(0)
    , m_timeSeriesXmax(FFT_SIZE),
    m_verticalCursorLine(new QGraphicsLineItem()),
    m_currentSampleRate(0)
{
    //m_pIqReceiver = new IqReceiver(2048);
    ui->setupUi(this);

    initializeWindow();
    initialiseRadio();
    //initializeAudio(QMediaDevices::defaultAudioInput());
    initializeAudio();

    //m_iqProcessor.start();
    //QThreadPool::globalInstance()->start(m_pIqReceiver);

//    QLineSeries *series = new QLineSeries();
//    series->append(0, 6);
//    series->append(2, 4);
//    series->append(3, 8);
//    series->append(7, 4);
//    series->append(10, 5);
//    *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

    //ui->panadapterView->chart()
    //QChart* pChart = ui->panadapterView->chart();

    ui->panadapterView->setRenderHint(QPainter::Antialiasing);
    ui->timeseriesView->setRenderHint(QPainter::Antialiasing);

   //pChart->legend()->hide();

    //pChart->addSeries(&m_fftSeries);
    //pChart->addSeries(&m_realSeries);
    //pChart->addSeries(&m_imagSeries);
    //QPen pen = m_fftSeries.pen();
    //pen.setWidth(0);



    //pChart->createDefaultAxes();
    //pChart->axisY()->setRange(0, 25);
    //pChart->axisY()->setRange(0.005, 0.025);
    //pChart->axisX()->setRange(0, FFT_SIZE);
    //pChart->setTitle("FunCube FFT");
    configurePanadapter();
    configureTimeseriesChart();
}

MainWindow::~MainWindow()
{
  // delete m_pIqReceiver;
  delete m_pRadio;
  delete ui;
}

void
MainWindow::configurePanadapter()
{
  QChart* pChart = ui->panadapterView->chart();
//    m_spectrumAreaSeries.setUpperSeries(&m_spectrumLineSeries);

//    QGradient plotAreaGradient(QGradient::Preset::MorpheusDen);
//    QBrush plotAreaBrush(plotAreaGradient);
//    pChart->setBackgroundBrush(plotAreaBrush);

  //QPen pen(QRgb(0xccd0e1)); //0x0080ff
  QPen pen(QRgb(0x0080ff));
  pen.setWidth(0);
//  m_spectrumAreaSeries.setPen(pen);
  m_spectrumLineSeries.setPen(pen);

//    QGradient seriesGradient(QGradient::Preset::EternalConstance);
//    QBrush seriesBrush(seriesGradient);
//    m_spectrumAreaSeries.setBrush(seriesBrush);

//  pChart->addSeries(&m_spectrumAreaSeries);
  pChart->addSeries(&m_spectrumLineSeries);
  pChart->setTitle("Panadapter");
  pChart->createDefaultAxes();
  QList<QAbstractAxis*> xAxes = pChart->axes(Qt::Horizontal);
  if (!xAxes.isEmpty()) {
    QValueAxis *xAxis = qobject_cast<QValueAxis*>(xAxes.first());
    xAxis->setRange(m_panadapterXmin, m_panadapterXmax);
    xAxis->setLabelFormat(QString("%i"));
  }
  pChart->axes(Qt::Vertical).first()->setRange(-140, 0);

  pChart->legend()->hide();

  m_verticalCursorLine->setPen(QPen(Qt::red, 1.5, Qt::SolidLine));
  pChart->scene()->addItem(m_verticalCursorLine);
    /*
    pChart->addSeries(pSeries);
    pChart->legend()->hide();
    pChart->setTitle("Panadapter");
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Frequency");
    axisX->setLabelFormat("%i");
    //axisX->setTickCount(pSeries->count());
    axisX->setRange(0, FFT_SIZE);
    pChart->addAxis(axisX, Qt::AlignBottom);
    pSeries->attachAxis(axisX);
    */
    /*
    //QLogValueAxis *axisY = new QLogValueAxis();
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Level");
    axisY->setLabelFormat("%g");
    //axisY->setBase(10.0);
    axisY->setRange(-140, 0);
    //axisY->setMinorTickCount(-1);
    pChart->addAxis(axisY, Qt::AlignLeft);
    pSeries->attachAxis(axisY);
    */
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
  pChart->setTitle("Timeseries");
  pChart->createDefaultAxes();
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
    handleRadioSettingsEvent(radioSettingsEvent->getSettings());
  }
}

void
MainWindow::handleReceiverIqEvent(const vsdrcomplex* data, uint32_t length, uint32_t sampleRate)
{
  m_currentSampleRate = sampleRate;
  vsdrreal spectrum(length);
  powerSpectrum(*data, length, spectrum);
  // if (spectrum.size() != m_panadapterXmax) {
  //   setPanadapterX(0, spectrum.size());
  // }
  uint32_t centreFrequency = m_radioSettings.rxSettings.rfSettings.frequency;
  uint32_t xMin = centreFrequency - (sampleRate / 2);
  uint32_t xMax = centreFrequency + (sampleRate / 2);
  if (m_panadapterXmin != xMin || m_panadapterXmax != xMax) {
    setPanadapterX(xMin, xMax);
  }
  replaceSpectrumSeries(&spectrum, m_spectrumLineSeries, sampleRate, true);
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
MainWindow::handleRadioSettingsEvent(const RadioSettings& radioSettings)
{
  m_radioSettings = radioSettings;
  if (m_radioSettings.rxSettings.rfSettings.changed & RfSettings::Features::OFFSET) {
    uint32_t centreFrequency = m_radioSettings.rxSettings.rfSettings.frequency;
    int32_t offset = m_radioSettings.rxSettings.rfSettings.offset;
  
    uint32_t frequencyAtOffset = centreFrequency + offset;
    QChart *chart = ui->panadapterView->chart();
    auto *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
    double yMin = axisY->min();
    double yMax = axisY->max();

    qDebug() << "Centre frequency changed to" << centreFrequency << "Offset" << offset << "Frequency at offset" << frequencyAtOffset;

    // Map chart coords back to pixel positions
    QPointF p1 = chart->mapToPosition(QPointF(frequencyAtOffset, yMin));
    QPointF p2 = chart->mapToPosition(QPointF(frequencyAtOffset, yMax));
    m_verticalCursorLine->setLine(QLineF(p1, p2));
    m_verticalCursorLine->show();

  }
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
  bool shuffle
)
{
  uint32_t centreFrequency = m_radioSettings.rxSettings.rfSettings.frequency;
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
  bool shuffle
)
{
  uint32_t centreFrequency = m_radioSettings.rxSettings.rfSettings.frequency;
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

void MainWindow::initializeWindow()
{
    // const QAudioDevice &defaultOutputDeviceInfo = QMediaDevices::defaultAudioOutput();
//
//    ui->deviceList->addItem(defaultDeviceInfo.description(), QVariant::fromValue(defaultDeviceInfo));
//    for (auto &deviceInfo : m_devices->audioInputs()) {
//        if (deviceInfo != defaultDeviceInfo)
//            ui->deviceList->addItem(deviceInfo.description(), QVariant::fromValue(deviceInfo));
//    }
//
//    connect(ui->deviceList, &QComboBox::activated, this, &MainWindow::deviceChanged);

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

    // RadioSettings radioSettings = {
    //   .rxSettings = {
    //     .rfSettings = { .frequency = 10000000, .gain = 0.0, .changed = (RfSettings::FREQUENCY | RfSettings::GAIN)},
    //     .ifSettings = { .bandwidth = 200000, .gain = 0.0, .changed = (IfSettings::BANDWIDTH | IfSettings::GAIN) },
    //     .changed = (ReceiverSettings::RF | ReceiverSettings::IF)
    //    },
    //   .changed = (RadioSettings::RX)
    // };
    m_radioSettings.rxSettings.rfSettings.frequency = 14140000;
    m_radioSettings.rxSettings.rfSettings.offset = -0;
    m_radioSettings.rxSettings.rfSettings.gain = 30.0;
    m_radioSettings.rxSettings.rfSettings.changed = (RfSettings::FREQUENCY | RfSettings::OFFSET | RfSettings::GAIN);
    m_radioSettings.rxSettings.ifSettings.bandwidth = 200000;
    m_radioSettings.rxSettings.ifSettings.gain = 0.0;
    m_radioSettings.rxSettings.ifSettings.changed = (IfSettings::BANDWIDTH | IfSettings::GAIN);
    m_radioSettings.rxSettings.changed = (ReceiverSettings::RF | ReceiverSettings::IF);
    m_radioSettings.changed = RadioSettings::RX;

    m_pRadio->applySettings(m_radioSettings);
  }
  catch (std::runtime_error& error)
  {
    qDebug() << "Error initialising radio: " << error.what();
  }
}

//#include "moc_mainwindow.cpp"

