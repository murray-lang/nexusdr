//
// Created by murray on 20/11/25.
//

#include "IqIo.h"

#include "dsp/iq/IqSourceFactory.h"

void IqIo::configure(const IqIoConfig* pConfig)
{
  delete m_pIqSource;
  delete m_pAudioOutput;
  m_pIqSource = nullptr;
  m_pAudioOutput = nullptr;

  m_pIqSource = IqSourceFactory::create(pConfig->getIqSource());

  auto audioOutputConfig = dynamic_cast<const AudioConfig*>(pConfig->getAudioOutput());
  m_pAudioOutput = new AudioOutput();
  m_pAudioOutput->configure(audioOutputConfig);
}

void
IqIo::setIqSink(IqSink* pIqSink)
{
  if (m_pIqSource != nullptr) {
    m_pIqSource->setIqSink(pIqSink);
  }
}

uint32_t
IqIo::sinkAudio(const vsdrreal& samples, uint32_t length, uint32_t numChannels)
{
  if (m_pAudioOutput != nullptr) {
    return m_pAudioOutput->sinkAudio(samples, length, numChannels);
  }
  return 0;
}

uint32_t
IqIo::getInputSampleRate() const
{
  if (m_pIqSource == nullptr) {
    return 0;
  }
  return m_pIqSource->getSampleRate();
}

uint32_t
IqIo::getOutputSampleRate() const
{
  if (m_pAudioOutput == nullptr) {
    return 0;
  }
  return m_pAudioOutput->getSampleRate();
}

void
IqIo::start(uint32_t maxFramesPerInputPacket, uint32_t maxFramesPerOutputPacket) const
{
  if (m_pAudioOutput != nullptr) {
    m_pAudioOutput->start(maxFramesPerOutputPacket);
  }
  if (m_pIqSource != nullptr) {
    m_pIqSource->start(maxFramesPerInputPacket);
  }
}

void
IqIo::stop() const
{
  if (m_pIqSource != nullptr) {
    m_pIqSource->stop();
  }
  if (m_pAudioOutput != nullptr) {
    m_pAudioOutput->stop();
  }
}
