//
// Created by murray on 20/11/25.
//

#include "IqTxPipeline.h"

#define FFT_SIZE 2048
#define PING_PONG_LENGTH 8192
#define HILBERT_TAPS 63

IqTxPipeline::IqTxPipeline() :
  m_pModulator(nullptr),
  m_resampler(),
  m_ifFilter(FFT_SIZE),
  m_inputSampleRate(0),
  m_outputSampleRate(0)
{
  addStage(&m_ifFilter);
  addStage(&m_resampler);
  addStage(&m_oscillatorMixer);
}

void
IqTxPipeline::initialise(IqIo* pIo, AudioSink* pAudioSink)
{
  IqPipeline::initialise(pIo, pAudioSink);
  m_inputSampleRate = pIo->getInputSampleRate();
  uint32_t preferredOutputRate = pIo->getOutputSampleRate();
  setOutputSampleRate(preferredOutputRate);
}

void
IqTxPipeline::setOutputSampleRate(uint32_t outputSampleRate)
{
  m_outputSampleRate = outputSampleRate;
  m_resampler.configure(m_inputSampleRate, outputSampleRate);
  m_oscillatorMixer.initialise(outputSampleRate, 0);
}

uint32_t
IqTxPipeline::getMaxFramesPerInputPacket() const
{
  uint32_t outputMaxFrames = getMaxFramesPerOutputPacket();
  return outputMaxFrames * m_inputSampleRate / m_outputSampleRate;
}

uint32_t
IqTxPipeline::getMaxFramesPerOutputPacket() const
{
  return m_buffers.getSize() / 2; // 2 is the number of channels (I + Q)
}

void IqTxPipeline::apply(const TransmitterSettings& settings)
{
  std::lock_guard<std::mutex> lock(m_settingsMutex);
  if (settings.changed & TransmitterSettings::RF) {
    if (settings.rfSettings.changed & RfSettings::OFFSET) {
      m_oscillatorMixer.setFrequency(-settings.rfSettings.offset);
    }
  }
  if (settings.changed & TransmitterSettings::MODE) {
    setMode(settings.mode);
  }
}

void IqTxPipeline::setMode(const Mode& mode)
{

  IqPipeline::setMode(mode);
  m_ifFilter.getKernel().configure(mode.getLoCut(), mode.getHiCut(), mode.getOffset(), m_inputSampleRate * 2);
  setModulator(mode.getType());
}

void
IqTxPipeline::setModulator(Mode::Type modeType)
{

  switch (modeType) {
  case Mode::Type::AMN:
  case Mode::Type::AMW:
    m_pModulator = nullptr;
    break;
  case Mode::Type::FMN:
  case Mode::Type::FMW:
    m_pModulator = nullptr;
    break;
  case Mode::Type::USB:
    m_pModulator = &m_ssbModulator;
    m_ssbModulator.setMode(SsbModulator::Mode::USB);
    break;
  case Mode::Type::LSB:
    m_pModulator = &m_ssbModulator;
    m_ssbModulator.setMode(SsbModulator::Mode::LSB);
    break;
  default:
    m_pModulator = nullptr;
    throw SettingsException("Unknown mode type");
    break;
  }
}


uint32_t
IqTxPipeline::sinkIq(const vsdrcomplex& samples, uint32_t length)
{
  uint32_t outputLength = 0;
  std::lock_guard<std::mutex> lock(m_settingsMutex);
  if (m_pModulator) {
    outputLength = m_pModulator->processSamples(samples, m_buffers.input(), length);
  }
  if (outputLength > 0) {
    for (auto stage : m_stages) {
      outputLength = stage->processSamples(m_buffers, outputLength);
      m_buffers.flip();
    }
  }
  if (outputLength > 0 && m_pAudioOutSink != nullptr) {
    uint32_t numReals = interleaveComplexToReal(m_buffers.input(), m_audioBuffer, outputLength);
    m_pAudioOutSink->sinkAudio(m_audioBuffer, numReals);
  }
  return outputLength;
}

uint32_t
IqTxPipeline::interleaveComplexToReal(const vsdrcomplex& vcomplex, vsdrreal& vreal, uint32_t numComplexes)
{
  vreal.resize(numComplexes * 2);
  std::memcpy(vreal.data(), vcomplex.data(), numComplexes * sizeof(sdrcomplex));
  return numComplexes * 2;
}

