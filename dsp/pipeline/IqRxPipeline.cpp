//
// Created by murray on 18/11/25.
//
#include "IqRxPipeline.h"

#define FFT_SIZE 2048
#define PING_PONG_LENGTH 8192
#define DEFAULT_SAMPLE_RATE 48000

#include "settings/ModeSettings.h"
#include "settings/RxPipelineSettings.h"

IqRxPipeline::IqRxPipeline(QObject* eventTarget) :
  IqPipeline(eventTarget),
  m_ifFilter(FFT_SIZE),
  m_amDemodulator(ModeSettings::getModeByType(Mode::AMN), DEFAULT_SAMPLE_RATE),
  m_fmnDemodulator(ModeSettings::getModeByType(Mode::FMN),DEFAULT_SAMPLE_RATE),
  m_fmwDemodulator(ModeSettings::getModeByType(Mode::FMW),DEFAULT_SAMPLE_RATE),
  m_ssbDemodulator(ModeSettings::getModeByType(Mode::USB),DEFAULT_SAMPLE_RATE),
  m_cwDemodulator(ModeSettings::getModeByType(Mode::CWU),DEFAULT_SAMPLE_RATE),
  m_pDemodulator(nullptr),
  m_audioBuffer(PING_PONG_LENGTH),
  m_pMonitoringStage(nullptr)
{
  m_pMonitoringStage = new MonitoringStage(eventTarget, ReceiverIqEvent::RxIqEvent, [this]() { return this->m_inputSampleRate; });

  addStage(m_pMonitoringStage);
  addStage(&m_iqCorrection);
  // addStage(m_pMonitoringStage);
  addStage(&m_oscillatorMixer);
  addStage(&m_ifFilter);
  addStage(&m_decimator);
  // addStage(&m_ifFilter);

}

IqRxPipeline::~IqRxPipeline() 
{
  delete m_pMonitoringStage;
}

void
IqRxPipeline::initialise(IqIo* pIo, AudioSink* pAudioSink)
{
  IqPipeline::initialise(pIo, pAudioSink);
  m_inputSampleRate = pIo->getInputSampleRate();
  m_oscillatorMixer.initialise(m_inputSampleRate, 0);
  uint32_t preferredOutputRate = pIo->getOutputSampleRate();
  setOutputSampleRate(preferredOutputRate);
}

void
IqRxPipeline::setOutputSampleRate(uint32_t preferredOutputRate)
{
  m_outputSampleRate = m_decimator.configure(m_inputSampleRate, preferredOutputRate);
  m_amDemodulator.setSampleRate(m_outputSampleRate);
  m_fmnDemodulator.setSampleRate(m_outputSampleRate);
  m_ssbDemodulator.setSampleRate(m_outputSampleRate);
}

uint32_t
IqRxPipeline::getMaxFramesPerInputPacket() const
{
  return m_buffers.getSize() / 2; // 2 is the number of channels (I + Q)
}

uint32_t
IqRxPipeline::getMaxFramesPerOutputPacket() const
{
  uint32_t inputMaxFrames = getMaxFramesPerInputPacket();
  return inputMaxFrames * m_outputSampleRate / m_inputSampleRate;
}

void
IqRxPipeline::apply(const ReceiverSettings& settings)
{
  std::lock_guard<std::mutex> lock(m_settingsMutex);
  if (settings.hasSettingChanged(ReceiverSettings::CORRECTION)) {
    m_iqCorrection.apply(settings.getCorrectionSettings());
  }
}

void
IqRxPipeline::apply(const RxPipelineSettings* settings)
{
  if (settings != nullptr) {
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    if (settings->hasSettingChanged(PipelineSettings::RF)) {
      if (settings->getRfSettings().hasSettingChanged(RfSettings::OFFSET)) {
        m_oscillatorMixer.setFrequency(-settings->getRfSettings().getOffset());
      }
    }
    if (settings->hasSettingChanged(PipelineSettings::MODE)) {
      setMode(settings->getMode());
    }
  }
}

void
IqRxPipeline::setMode(const Mode& mode)
{

  IqPipeline::setMode(mode);
  // const uint32_t decimatorOutputRate = m_decimator.getOutputSampleRate();
  m_ifFilter.getKernel().configure(mode.getLoCut(), mode.getHiCut(), mode.getOffset(), m_inputSampleRate);
  setDemodulator(mode);
}

void
IqRxPipeline::setDemodulator(const Mode& mode)
{
  switch (mode.getType()) {
  case Mode::Type::AMN:
  case Mode::Type::AMW:
    m_pDemodulator = &m_amDemodulator;
    break;
  case Mode::Type::FMN:
    m_pDemodulator = &m_fmnDemodulator;
    break;
  case Mode::Type::FMW:
    m_pDemodulator = &m_fmwDemodulator;
    break;
  case Mode::Type::USB:
  case Mode::Type::LSB:
    m_pDemodulator = &m_ssbDemodulator;
    break;
  case Mode::Type::CWU:
  case Mode::Type::CWL:
    m_pDemodulator = &m_cwDemodulator;
    break;
  default:
    m_pDemodulator = nullptr;
    throw SettingsException("Unknown mode type");
    break;
  }
  if (m_pDemodulator != nullptr) {
    m_pDemodulator->setMode(mode);
  }
}

uint32_t
IqRxPipeline::sinkIq(const vsdrcomplex& samples, uint32_t length)
{
  uint32_t outputLength = length;

  vsdrcomplex& input = m_buffers.input();
  std::copy(samples.begin(), samples.end(), input.begin());
  std::lock_guard<std::mutex> lock(m_settingsMutex);
  for (auto stage : m_stages) {
    outputLength = stage->processSamples(m_buffers, outputLength);
    m_buffers.flip();
  }
  if (m_pDemodulator != nullptr) {
      outputLength = m_pDemodulator->processSamples(m_buffers.input(), m_audioBuffer, outputLength);
  } else {
    outputLength = 0;
  }
  if (outputLength > 0 && m_pAudioOutSink != nullptr) {
    m_pAudioOutSink->sinkAudio(m_audioBuffer, outputLength, m_pDemodulator->getNumOutputChannels());
  }
  return outputLength;
}