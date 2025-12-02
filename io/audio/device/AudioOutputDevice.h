//
// Created by murray on 18/07/25.
//

#pragma once

#include <deque>
#include <QIODevice>
#include <mutex>


#include "AudioDevice.h"
#include "../../../SampleTypes.h"

template <typename T>
class AudioOutputDevice : public AudioDevice
{
public:

  AudioOutputDevice(const RtAudio::DeviceInfo& deviceInfo, const Format& format) :
    AudioDevice(deviceInfo, format),
    m_running(false),
    m_audioBuffer(),
    m_maxPacketFrames(0)
  {
  }
  ~AudioOutputDevice() override
  {
    AudioOutputDevice::stop();
  };

  void AudioOutputDevice::start(uint32_t maxPacketFrames)
  {
    m_maxPacketFrames = maxPacketFrames;
    if (!m_running) {
      m_running = true;
      RtAudio::StreamParameters parameters;
      parameters.deviceId = m_deviceInfo.ID;
      parameters.nChannels = 2; //std::min(m_deviceInfo.outputChannels, static_cast<unsigned int>(2));
      parameters.firstChannel = 0;

      // RtAudio::StreamOptions options{
      //   .flags = 0, //RTAUDIO_NONINTERLEAVED,
      //   .numberOfBuffers = 2
      // };
      // Set options as needed

      unsigned int sampleRate = m_format.sampleRate;

      RtAudioCallback rtCallback = [](void *outputBuffer, void *, unsigned int nFrames,
                          double, RtAudioStreamStatus, void *userData) -> int {
        auto *self = static_cast<AudioOutputDevice *>(userData);
        return self->pullSamples(outputBuffer, nFrames);
      };

      RtAudioErrorType rc = m_rtAudio.openStream(
        &parameters,
        nullptr,
        m_format.sampleFormat,
        sampleRate,
        &m_maxPacketFrames, rtCallback, this /*, &options*/);

      rc = m_rtAudio.startStream();
    }

}

void stop()
{
  if (m_running) {
    if (m_rtAudio.isStreamOpen()) {
      m_rtAudio.stopStream();
      m_rtAudio.closeStream();
      m_running = false;
    }
  }
}

int pullSamples(void *outputBuffer, unsigned int nFrames)
{
  static size_t numUnderruns = 0;
  auto* out = static_cast<T*>(outputBuffer);
  std::lock_guard<std::mutex> lock(m_mutex);
  for (unsigned int i = 0; i < nFrames * 2; i += 2) {
    if (!m_audioBuffer.empty()) {
      out[i] = m_audioBuffer.front();
      out[i+1] = out[i];
      m_audioBuffer.pop_front();
    } else {
      // qDebug() << "AudioOutputDevice::pullSamples(): underrun! " << ++numUnderruns;
      out[i] = 0; // silence if no data
      out[i+1] = 0;
    }
  }
  return 0; // 0: continue, nonzero: stop
}



uint32_t addAudioData(const vsdrreal& data, const uint32_t length)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  std::transform(
    data.begin(),
    data.begin() + length,
    std::back_inserter(m_audioBuffer),
    [this](const sdrreal& value)
    {
      if (m_format.sampleFormat == RTAUDIO_SINT16) {
        // Scale float [-1.0, 1.0] to int16_t [-32768, 32767]
        sdrreal scaled = value * static_cast<int16_t>(INT16_MAX);
        return static_cast<int16_t>(scaled);
      } else if(m_format.sampleFormat == RTAUDIO_SINT8) {
        // Scale float [-1.0, 1.0] to int8_t [-128, 127]
        sdrreal scaled = value * static_cast<int8_t>(INT8_MAX);
        return static_cast<int16_t>(scaled);
      } else if(m_format.sampleFormat == RTAUDIO_SINT24) {
        // Scale float [-1.0, 1.0] to int24_t [-8388608, 8388607]
        sdrreal scaled = value * 8388607.0f;
        int32_t int24 = static_cast<int32_t>(scaled);
        // Store as int16_t by shifting (not ideal, but for demonstration)
        return static_cast<int16_t>(int24 >> 8);
      } else if(m_format.sampleFormat == RTAUDIO_SINT32) {
        // Scale float [-1.0, 1.0] to int32_t [-2147483648, 2147483647]
        sdrreal scaled = value * static_cast<int32_t>(INT32_MAX);
        int32_t int32Value = static_cast<int32_t>(scaled);
        // Store as int16_t by shifting (not ideal, but for demonstration)
        return static_cast<int16_t>(int32Value >> 16);
      // For other formats, just return the float value as is (may need adjustment)
      sdrreal scaled = value * static_cast<int16_t>(INT16_MAX);
      return static_cast<int16_t>(scaled);
      // return static_cast<float>(value);
    }
  );
  return length;
}
private:
  std::atomic<bool> m_running;

  std::deque<T> m_audioBuffer;
  std::mutex m_mutex;

  uint32_t m_maxPacketFrames;

};
