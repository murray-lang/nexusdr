//
// Created by murray on 11/11/25.
//

#include "IqTransmitter.h"
#include "TransmitterAudioEvent.h"
#include <qcoreapplication.h>

#include "TransmitterIqEvent.h"


IqTransmitter::IqTransmitter(QObject* eventTarget) :
  m_eventTarget(eventTarget)
{
}

void
IqTransmitter::configure(const TransmitterConfig* pConfig)
{
  // if (pConfig != nullptr) {
  //   m_iqIo.configure(&pConfig->iqIo);
  //   m_iqPipeline.initialise(&m_iqIo, &m_iqIo);
  //   m_iqIo.setIqSink(&m_iqPipeline);
  // }
  if (pConfig != nullptr) {
    m_iqIo.configure(&pConfig->iqIo);
    // The AudioSink provided here could be m_pPipelineIo, but...
    // this class intercepts the audio out for display purposes.
    m_iqPipeline.initialise(&m_iqIo, dynamic_cast<AudioSink*>(this));
    // This class also intercepts the received IQ for display purposes.
    m_iqIo.setIqSink(this);
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

void
IqTransmitter::setMode(const Mode& mode)
{
  m_iqPipeline.setMode(mode);
}

uint32_t
IqTransmitter::sinkIq(const vsdrcomplex& samples, uint32_t length)
{
  QCoreApplication::postEvent(m_eventTarget, new TransmitterIqEvent(samples, length, m_iqIo.getInputSampleRate() ));

  return m_iqPipeline.sinkIq(samples, length);
}

uint32_t
IqTransmitter::sinkAudio(const vsdrreal& samples, uint32_t length)
{
  // QCoreApplication::postEvent(m_eventTarget, new TransmitterAudioEvent(samples, length));
  return m_iqIo.sinkAudio(samples, length);
}
