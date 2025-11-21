#include "IqReceiver.h"
#include <qdebug.h>
#include <cstdlib>
#include <complex>
#include <sstream>
#include <qcoreapplication.h>

#include "ReceiverAudioEvent.h"
#include "ReceiverIqEvent.h"
#include <config/ReceiverConfig.h>

// #define FFT_SIZE 2048


IqReceiver::IqReceiver(QObject* eventTarget) :
  m_eventTarget(eventTarget)
{
}

void
IqReceiver::configure(const ReceiverConfig* pConfig)
{
  if (pConfig != nullptr) {
    m_iqIo.configure(&pConfig->iqIo);
    // The AudioSink provided here could be m_pPipelineIo, but...
    // this class intercepts the audio out because once multiple pipelines are implemented, then the audio from
    // each will need to be mixed somehow before being sent to I/O. Maybe the mixing could be handled
    // there, but not sure how it's going to work at this stage.
    m_iqPipeline.initialise(&m_iqIo, dynamic_cast<AudioSink*>(this));
    // This class also intercepts the received IQ so that it can be forwarded to multiple pipelines.
    m_iqIo.setIqSink(this);
  }
}

void IqReceiver::apply(const ReceiverSettings& settings)
{
  m_iqPipeline.apply(settings);
}

void
IqReceiver::start()
{

  uint32_t framesPerOutputPacket = m_iqPipeline.getMaxFramesPerOutputPacket();
  uint32_t framesPerInputPacket = m_iqPipeline.getMaxFramesPerInputPacket();
  m_iqIo.start(framesPerInputPacket, framesPerOutputPacket);
}

void
IqReceiver::stop()
{
  m_iqIo.stop();
}

void IqReceiver::ptt(bool on)
{
  if (on) {
    stop();
  } else {
    start();
  }
}

uint32_t
IqReceiver::sinkIq(const vsdrcomplex& samples, uint32_t length)
{
  QCoreApplication::postEvent(m_eventTarget, new ReceiverIqEvent(samples, length, m_iqIo.getInputSampleRate() ));
  // Straight to the only pipeline for now
  return m_iqPipeline.sinkIq(samples, length);
}

uint32_t
IqReceiver::sinkAudio(const vsdrreal& samples, uint32_t length)
{
  QCoreApplication::postEvent(m_eventTarget, new ReceiverAudioEvent(samples, length));
  // TODO: There will need to be a way to distinguish which pipeline this is from and also to combine the audio.
  return m_iqIo.sinkAudio(samples, length);
}
