//
// Created by murray on 20/11/25.
//

#include "IqIo.h"

#include "core/dsp/iq/IqSourceFactory.h"
#include "io/audio/AudioOutputFactory.h"

ResultCode IqIo::configure(const Config::IqIo::Fields& config)
{
  ResultCode rc = IqSourceFactory::create(config.iqSource, m_iqSource);
  if (rc != ResultCode::OK) return rc;

  return AudioOutputFactory::create(config.audioOutput, m_audioOutput);
}

void
IqIo::setIqSink(IqSink* pIqSink)
{
  visit([pIqSink](auto&& arg)
  {
    using T = decay_t<decltype(arg)>;
    if constexpr (!is_same_v<T, monostate>) {
      arg.setIqSink(pIqSink);
    }
  }, m_iqSource);
}

uint32_t
IqIo::sinkAudio(const RealSamplesMax& samples, uint32_t length, uint32_t numChannels)
{
  if (auto* pAudioOutput = get_if<AudioOutput>(&m_audioOutput)) {
    return pAudioOutput->sinkAudio(samples, length, numChannels);
  }
  return 0;
}

uint32_t
IqIo::getInputSampleRate() const
{
  return visit([](const auto& arg) -> uint32_t {
    using T = decay_t<decltype(arg)>;

    if constexpr (is_same_v<T, monostate>) {
      return 0;
    } else {
      return arg.getSampleRate();
    }
  }, m_iqSource);
}

uint32_t
IqIo::getOutputSampleRate() const
{
  return visit([](const auto& arg) -> uint32_t
  {
    using T = decay_t<decltype(arg)>;

    if constexpr (is_same_v<T, monostate>) {
      return 0;
    } else {
      return arg.getSampleRate();
    }
  }, m_audioOutput);
}

ResultCode
IqIo::start(uint32_t maxFramesPerInputPacket, uint32_t maxFramesPerOutputPacket)
{
  ResultCode rc = startOutput(maxFramesPerOutputPacket);
  if (rc != ResultCode::OK) return rc;
  return startInput(maxFramesPerInputPacket);
}

void
IqIo::stop()
{
  stopInput();
  stopOutput();
}

ResultCode
IqIo::startInput(uint32_t maxFramesPerInputPacket)
{
  return visit([maxFramesPerInputPacket](auto& arg) -> ResultCode {
    using T = decay_t<decltype(arg)>;

    if constexpr (is_same_v<T, monostate>) {
      return ResultCode::ERR_IQ_SOURCE_MISSING;
    } else {
      return arg.start(maxFramesPerInputPacket);
    }
  }, m_iqSource);
}

ResultCode
IqIo::startOutput(uint32_t maxFramesPerOutputPacket)
{
  return visit([maxFramesPerOutputPacket](auto& arg) -> ResultCode {
    using T = decay_t<decltype(arg)>;

    if constexpr (is_same_v<T, monostate>) {
      return ResultCode::ERR_AUDIO_OUTPUT_MISSING;
    } else {
      return arg.start(maxFramesPerOutputPacket);
    }
  }, m_audioOutput);
}

void
IqIo::stopInput()
{
  visit([](auto& arg) -> void {
    using T = decay_t<decltype(arg)>;
    if constexpr (!is_same_v<T, monostate>) {
      arg.stop();
    }
  }, m_iqSource);
}

void
IqIo::stopOutput()
{
  visit([](auto& arg) -> void {
    using T = decay_t<decltype(arg)>;
    if constexpr (!is_same_v<T, monostate>) {
      arg.stop();
    }
  }, m_audioOutput);
}
