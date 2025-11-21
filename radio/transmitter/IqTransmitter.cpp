//
// Created by murray on 11/11/25.
//

#include "IqTransmitter.h"
#include "TransmitterAudioEvent.h"


IqTransmitter::IqTransmitter(QObject* eventTarget) :
  m_eventTarget(eventTarget)
{
}

void
IqTransmitter::configure(const TransmitterConfig* pConfig)
{
  if (pConfig != nullptr) {
    m_iqIo.configure(&pConfig->iqIo);
    // Send the audio (interleaved I/Q) from the pipeline directly to the output since there is only one
    // TX pipeline (no mixing to be done beforehand as with the receiver).
    m_iqPipeline.initialise(&m_iqIo, &m_iqIo);
    m_iqIo.setIqSink(&m_iqPipeline);
  }
}
void
IqTransmitter::apply(const TransmitterSettings& settings)
{
  m_iqPipeline.apply(settings);
}

void
IqTransmitter::start() const
{
  uint32_t framesPerOutputPacket = m_iqPipeline.getMaxFramesPerOutputPacket();
  uint32_t framesPerInputPacket = m_iqPipeline.getMaxFramesPerInputPacket();
  m_iqIo.start(framesPerInputPacket, framesPerOutputPacket);
}

void
IqTransmitter::stop() const
{
  m_iqIo.stop();
}

void IqTransmitter::ptt(bool on)
{
  if (on) {
    start();
  } else {
    stop();
  }
}
