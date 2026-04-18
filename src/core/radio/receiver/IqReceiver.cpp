#include "IqReceiver.h"
#include <qdebug.h>
#include <cstdlib>
#include <complex>
#include <sstream>
#include <qcoreapplication.h>

#include "events/ReceiverAudioEvent.h"
#include "events/ReceiverIqEvent.h"
#include "../../config-settings/config/receiver/ReceiverConfig.h"

#include "core/config-settings/settings/bands/BandSettings.h"

// #define FFT_SIZE 2048


IqReceiver::IqReceiver(QObject* eventTarget) :
  m_iqPipelineA(eventTarget),
  m_iqPipelineB(eventTarget),
  m_focusPipelineId(PipelineId::A),
  m_eventTarget(eventTarget),
  m_audioTapA(*this, PipelineId::A),
  m_audioTapB(*this, PipelineId::B),
  m_pipelineBEnabled(false),
  m_havePendingA(false),
  m_havePendingB(false),
  m_pendingA(0.0),
  m_pendingB(0.0),
  m_stereoOutInterleaved(0.0)
{
}

void
IqReceiver::configure(const ReceiverConfig* pConfig)
{
  if (pConfig != nullptr) {
    m_iqIo.configure(&pConfig->iqIo);

    m_iqPipelineA.initialise(&m_iqIo, &m_audioTapA);
    m_iqPipelineB.initialise(&m_iqIo, &m_audioTapB);
    // This class also intercepts the received IQ so that it can be forwarded to multiple pipelines.
    m_iqIo.setIqSink(this);
  }
}

void
IqReceiver::apply(const ReceiverSettings& settings)
{
  m_iqPipelineA.apply(settings);
  m_iqPipelineB.apply(settings);
}

void
IqReceiver::apply(BandSettings* bandSettings)
{
  if (bandSettings == nullptr) {
    return;
  }
  bool multiPipelineChanged = false;
  if (bandSettings->hasSettingChanged(BandSettings::MULTI_PIPELINE)) {
    bool newMultiPipeline = bandSettings->isMultiPipeline();
    if (newMultiPipeline != m_pipelineBEnabled) {
      multiPipelineChanged = true;
      m_pipelineBEnabled = newMultiPipeline;
    }
  }
  if (multiPipelineChanged) {
    RxPipelineSettings* rxPipelineASettings = bandSettings->getRxPipeline(PipelineId::A);
    adjustRfSettingsToLimits(rxPipelineASettings, m_iqPipelineA);
    m_iqPipelineA.apply(rxPipelineASettings);

    if (m_pipelineBEnabled) {
      RxPipelineSettings* rxPipelineBSettings = bandSettings->getRxPipeline(PipelineId::B);
      adjustRfSettingsToLimits(rxPipelineBSettings, m_iqPipelineB);
      m_iqPipelineB.apply(rxPipelineBSettings);
    }
  } else if (bandSettings->hasSettingChanged(BandSettings::WITH_FOCUS_PIPELINE)) {
    IqRxPipeline* focusPipeline = m_focusPipelineId == PipelineId::A ? &m_iqPipelineA : &m_iqPipelineB;
    RxPipelineSettings* focusPipelineSettings = bandSettings->getFocusPipeline();
    adjustRfSettingsToLimits(focusPipelineSettings, *focusPipeline);
    focusPipeline->apply(focusPipelineSettings);
  }
 
  if (bandSettings->hasSettingChanged(BandSettings::FOCUS_PIPELINE)) {
    PipelineId newFocusPipelineId = bandSettings->getFocusPipelineId();
    m_iqPipelineA.monitor(newFocusPipelineId == PipelineId::A);
    if (m_pipelineBEnabled) {
      m_iqPipelineB.monitor(newFocusPipelineId == PipelineId::B);
    }
    if (newFocusPipelineId != m_focusPipelineId) {
      m_focusPipelineId = newFocusPipelineId;
    }
  }
}

bool IqReceiver::adjustRfSettingsToLimits(RxPipelineSettings* rxPipelineSettings, IqRxPipeline& pipeline, bool onlyIfChanged) const
{
  if (rxPipelineSettings == nullptr) {
    return false;
  }
  RfSettings& rfSettings = rxPipelineSettings->getRfSettings();

  if (onlyIfChanged) {
    if (rfSettings.hasSettingChanged(RfSettings::CENTER_FREQUENCY)
      || rfSettings.hasSettingChanged(RfSettings::VFO)) {
      return pipeline.adjustRfSettingsToLimits(rfSettings);
      }
    return false;
  }
  return pipeline.adjustRfSettingsToLimits(rfSettings);
}

void
IqReceiver::start()
{

  uint32_t framesPerOutputPacket = m_iqPipelineA.getMaxFramesPerOutputPacket();
  uint32_t framesPerInputPacket = m_iqPipelineA.getMaxFramesPerInputPacket();
  m_iqIo.start(framesPerInputPacket, framesPerOutputPacket);
}

void
IqReceiver::stop()
{
  m_iqIo.stop();
}

void
IqReceiver::setMode(const Mode& mode)
{
  m_iqPipelineA.setMode(mode);
  if (m_pipelineBEnabled) {
    m_iqPipelineB.setMode(mode);
  }
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
IqReceiver::sinkIq(const ComplexSamplesMax& samples, uint32_t length)
{
  // EventDispatcher::postEvent(m_eventTarget, new ReceiverIqEvent(samples, length, m_iqIo.getInputSampleRate() ));

  // Always feed pipeline A
  uint32_t outA = m_iqPipelineA.sinkIq(samples, length);

  // Feed pipeline B only when enabled
  if (m_pipelineBEnabled) {
    (void)m_iqPipelineB.sinkIq(samples, length);
  }

  return outA;
}

uint32_t
IqReceiver::downmixToMono(const RealSamplesMax& in,
                          uint32_t length,
                          uint32_t numChannels,
                          RealSamplesMax& outMono)
{
  if (length == 0 || numChannels == 0) {
    outMono.clear();
    return 0;
  }

  const uint32_t frames = length / numChannels;
  outMono.resize(frames);

  if (frames == 0) {
    return 0;
  }

  if (numChannels == 1) {
    std::copy(in.begin(), in.begin() + frames, outMono.begin());
    return frames;
  }

  for (uint32_t f = 0; f < frames; ++f) {
    double acc = 0.0;
    const uint32_t base = f * numChannels;
    for (uint32_t c = 0; c < numChannels; ++c) {
      acc += in[base + c];
    }
    outMono[f] = static_cast<float>(acc / static_cast<double>(numChannels));
  }
  return frames;
}

void
IqReceiver::outputStereoFromMono(const RealSamplesMax& leftMono,
                                 const RealSamplesMax& rightMono,
                                 uint32_t frames)
{
  if (frames == 0) {
    return;
  }

  m_stereoOutInterleaved.resize(frames * 2);
  for (uint32_t i = 0; i < frames; ++i) {
    m_stereoOutInterleaved[i * 2 + 0] = leftMono[i];
    m_stereoOutInterleaved[i * 2 + 1] = rightMono[i];
  }

  (void)m_iqIo.sinkAudio(m_stereoOutInterleaved,
                        static_cast<uint32_t>(m_stereoOutInterleaved.size()),
                        2);
}

void
IqReceiver::outputStereoDuplicate(const RealSamplesMax& mono, uint32_t frames)
{
  if (frames == 0) {
    return;
  }

  m_stereoOutInterleaved.resize(frames * 2);
  for (uint32_t i = 0; i < frames; ++i) {
    const float s = mono[i];
    m_stereoOutInterleaved[i * 2 + 0] = s;
    m_stereoOutInterleaved[i * 2 + 1] = s;
  }

  (void)m_iqIo.sinkAudio(m_stereoOutInterleaved,
                        static_cast<uint32_t>(m_stereoOutInterleaved.size()),
                        2);
}

uint32_t
IqReceiver::sinkPipelineAudio(PipelineId pipelineId,
                              const RealSamplesMax& samples,
                              uint32_t length,
                              uint32_t numChannels)
{
  RealSamplesMax mono;
  const uint32_t frames = downmixToMono(samples, length, numChannels, mono);
  if (frames == 0) {
    return 0;
  }

  if (!m_pipelineBEnabled) {
    // A-only mode: duplicate to stereo. (If B accidentally calls, still harmless.)
    outputStereoDuplicate(mono, frames);
    return frames;
  }

  if (pipelineId == PipelineId::A) {
    m_pendingA.swap(mono);
    m_havePendingA = true;

    // If B already arrived first (unexpected), pair immediately.
    if (m_havePendingB) {
      const uint32_t n = std::min<uint32_t>(
        static_cast<uint32_t>(m_pendingA.size()),
        static_cast<uint32_t>(m_pendingB.size())
      );
      outputStereoFromMono(m_pendingA, m_pendingB, n);
      m_havePendingA = false;
      m_havePendingB = false;
    }
    return frames;
  }

  // PipelineId::B
  if (m_havePendingA) {
    const uint32_t n = std::min<uint32_t>(
      static_cast<uint32_t>(m_pendingA.size()),
      static_cast<uint32_t>(mono.size())
    );
    outputStereoFromMono(m_pendingA, mono, n);
    m_havePendingA = false;
    return frames;
  }

  // B arrived before A (should be rare). Keep it for pairing.
  m_pendingB.swap(mono);
  m_havePendingB = true;
  return frames;
}

// uint32_t
// IqReceiver::sinkAudio(const vsdrreal& samples, uint32_t length, uint32_t numChannels)
// {
//   EventDispatcher::postEvent(m_eventTarget, new ReceiverAudioEvent(samples, length));
//   // TODO: There will need to be a way to distinguish which pipeline this is from and also to combine the audio.
//   return m_iqIo.sinkAudio(samples, length, numChannels);
// }
