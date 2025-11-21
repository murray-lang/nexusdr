//
// Created by murray on 18/11/25.
//
#include "IqRxPipeline.h"

#define FFT_SIZE 2048
#define PING_PONG_LENGTH 8192

IqRxPipeline::IqRxPipeline() :
  IqPipeline(),
  m_ifFilter(FFT_SIZE),
  m_amDemodulator(48000),
  m_fmDemodulator(48000),
  m_ssbDemodulator(48000),
  m_pDemodulator(nullptr),
  m_audioBuffer(PING_PONG_LENGTH)
{
  addStage(&m_oscillatorMixer);
  addStage(&m_decimator);
  addStage(&m_ifFilter);

  m_ssbDemodulator.setMode(SsbDemodulator::Mode::USB);
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
  m_amDemodulator.setOutputRate(m_outputSampleRate);
  m_fmDemodulator.setOutputRate(m_outputSampleRate);
  m_ssbDemodulator.setOutputRate(m_outputSampleRate);
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

void IqRxPipeline::apply(const ReceiverSettings& settings)
{
  std::lock_guard<std::mutex> lock(m_settingsMutex);
  if (settings.changed & ReceiverSettings::RF) {
    if (settings.rfSettings.changed & RfSettings::OFFSET) {
      m_oscillatorMixer.setFrequency(-settings.rfSettings.offset);
    }
  }
  if (settings.changed & ReceiverSettings::MODE) {
    setMode(settings.mode);
  }
}

void
IqRxPipeline::setMode(const Mode& mode)
{

  IqPipeline::setMode(mode);
  const uint32_t decimatorOutputRate = m_decimator.getOutputSampleRate();
  m_ifFilter.getKernel().configure(mode.getLoCut(), mode.getHiCut(), mode.getOffset(), decimatorOutputRate * 2);
  setDemodulator(mode.getType());
}

void
IqRxPipeline::setDemodulator(Mode::Type modeType)
{

  switch (modeType) {
  case Mode::Type::AMN:
  case Mode::Type::AMW:
    m_pDemodulator = &m_amDemodulator;
    break;
  case Mode::Type::FMN:
  case Mode::Type::FMW:
    m_pDemodulator = &m_fmDemodulator;
    break;
  case Mode::Type::USB:
    m_pDemodulator = &m_ssbDemodulator;
    m_ssbDemodulator.setMode(SsbDemodulator::Mode::USB);
    break;
  case Mode::Type::LSB:
    m_pDemodulator = &m_ssbDemodulator;
    m_ssbDemodulator.setMode(SsbDemodulator::Mode::LSB);
    break;
  default:
    m_pDemodulator = nullptr;
    throw SettingsException("Unknown mode type");
    break;
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
    m_pAudioOutSink->sinkAudio(m_audioBuffer, outputLength);
  }
  return outputLength;
}