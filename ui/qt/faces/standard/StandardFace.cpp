//
// Created by murray on 27/1/26.
//

#include "StandardFace.h"
#include "ui_StandardFace.h"          // generated from StandardFace.ui
#include "../RegisterFace.h"
#include "../../common/QtPanadapter.h"
#include "../../common/QtTimeSeriesChart.h"

StandardFace::StandardFace(QWidget* parent)
  : FaceBase(parent)
  , ui(std::make_unique<Ui::StandardFace>())
  , m_pRadio(nullptr)
  , m_pTimeSeriesChart(nullptr)
  , m_pPanadapter(nullptr)
  , m_reportedIqSampleRate(0)
{
  ui->setupUi(this);
}

StandardFace::~StandardFace()
{
  delete m_pPanadapter;
  delete m_pTimeSeriesChart;
}

void
StandardFace::initialise(RadioSettings* pRadioSettings)
{
  FaceBase::initialise(pRadioSettings);
  auto* chartTheme = new QtChartTheme(this);
  chartTheme->setObjectName("chartTheme");
  chartTheme->setFixedSize(0, 0); // User can't see it, but Style Engine will style it

  m_pPanadapter = new QtPanadapter(this, "panadapterView", "chartTheme");
  m_pPanadapter->initialise();

  m_pTimeSeriesChart = new QtTimeSeriesChart(this, "timeseriesView", "chartTheme");
  m_pTimeSeriesChart->initialise();

  ui->volumeSlider->setRange(0, 100);
  ui->volumeSlider->setValue(100);
}


void StandardFace::setRadio(Radio* radio) {
  FaceBase::setRadio(radio);
}

void StandardFace::notifyRadioSettingsChanged() {

  BandSettings* bandSettings = m_pRadioSettings->getFocusBandSettings();
  RxPipelineSettings* rxPipelineSettings = bandSettings->getFocusRxPipelineSettings();
  if (rxPipelineSettings == nullptr) {
    return;
  }
  RfSettings& rfSettings = rxPipelineSettings->getRfSettings();
  auto centreFrequency = static_cast<int32_t>(rfSettings.getCentreFrequency());
  int32_t vfo = rfSettings.getVfo();

  ui->centreFrequencyLcd->display(centreFrequency);
  ui->cursorFrequencyLcd->display(vfo);

  if (m_reportedIqSampleRate > 0) {
    uint32_t xMin = centreFrequency - (m_reportedIqSampleRate / 2);
    uint32_t xMax = centreFrequency + (m_reportedIqSampleRate / 2);
    m_pPanadapter->setSeriesXMinMax(xMin, xMax);
  }
  const Mode& mode = rxPipelineSettings->getMode();
  m_pPanadapter->updateCursorPosition(vfo, mode.getLoCut(), mode.getHiCut());
}

void
StandardFace::handleReceiverIq(const vsdrcomplex* data, uint32_t length, uint32_t sampleRate)
{
  m_reportedIqSampleRate = sampleRate;
  RxPipelineSettings* rxPipelineSettings = m_pRadioSettings->getFocusRxPipelineSettings();
  if (rxPipelineSettings != nullptr) {
    const RfSettings& rfSettings = rxPipelineSettings->getRfSettings();
    uint32_t centreFrequency = rfSettings.getCentreFrequency();
    uint32_t xMin = centreFrequency - (sampleRate / 2);
    uint32_t xMax = centreFrequency + (sampleRate / 2);

    m_pPanadapter->setSeriesXMinMax(xMin, xMax);
    m_pPanadapter->plot(data, length, sampleRate, centreFrequency, true);
  }
}

void
StandardFace::handleReceiverAudio(const vsdrreal* data, uint32_t length)
{
  m_pTimeSeriesChart->plot(*data, length);
}

void
StandardFace::handleTransmitterIq(const vsdrcomplex* data, uint32_t length, uint32_t sampleRate)
{
  m_reportedIqSampleRate = sampleRate;
  TxPipelineSettings* txPipelineSettings = m_pRadioSettings->getTxPipelineSettings();
  if (txPipelineSettings != nullptr) {
    const RfSettings& rfSettings = txPipelineSettings->getRfSettings();
    uint32_t centreFrequency = rfSettings.getCentreFrequency();
    uint32_t xMin = centreFrequency - (sampleRate / 2);
    uint32_t xMax = centreFrequency + (sampleRate / 2);

    m_pPanadapter->setSeriesXMinMax(xMin, xMax);
    m_pPanadapter->plot(data, length, sampleRate, centreFrequency, true);
  }

  m_pTimeSeriesChart->plot(*data, length);
}

void
StandardFace::handleTransmitterAudio(const vsdrreal* data, uint32_t length)
{
  m_pTimeSeriesChart->plot(*data, length);
}

REGISTER_FACE(StandardFace, "standard");