//
// Created by murray on 18/07/25.
//

#pragma once

#include <deque>
#include <QIODevice>
#include <mutex>


#include "AudioDevice.h"
#include "../../../SampleTypes.h"

class AudioOutputDevice : public AudioDevice
{
public:
  AudioOutputDevice(const RtAudio::DeviceInfo& deviceInfo, const Format& format) : AudioDevice(deviceInfo, format) {}
  ~AudioOutputDevice() override = default;
  virtual uint32_t addAudioData(const vsdrreal& data, uint32_t length) = 0;
};

template <typename T>
class AudioOutputDeviceT : public AudioOutputDevice
{
public:

  AudioOutputDeviceT(const RtAudio::DeviceInfo& deviceInfo, const Format& format) :
    AudioOutputDevice(deviceInfo, format),
    m_running(false),
    m_audioBuffer(),
    m_maxPacketFrames(0)
  {
  }
  ~AudioOutputDeviceT() override
  {
    AudioOutputDeviceT::stop();
  };

  void start(uint32_t maxPacketFrames) override
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
        auto *self = static_cast<AudioOutputDeviceT *>(userData);
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

  void stop() override
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
    std::lock_guard<std::mutex> lock(m_mutex);
    T* out = static_cast<T*>(outputBuffer);

    unsigned int samplesNeeded = nFrames * m_format.channelCount;

    unsigned int samplesToCopy = std::min(static_cast<unsigned int>(m_audioBuffer.size()), samplesNeeded);

    for (unsigned int i = 0; i < samplesToCopy; ++i) {
      *out++ = m_audioBuffer.front();
      m_audioBuffer.pop_front();
    }

    // Fill the rest with silence if we ran out of data (underrun protection)
    if (samplesToCopy < samplesNeeded) {
      std::fill(out, out + (samplesNeeded - samplesToCopy), static_cast<T>(0));
      // Optionally log underrun here
    }

    return 0; // 0: continue, nonzero: stop
  }

  uint32_t addAudioData(const vsdrreal& data, uint32_t length) override
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_running) return 0;

    constexpr double scale = std::is_integral_v<T>
        ? static_cast<double>(std::numeric_limits<T>::max())
        : 1.0;

    for (uint32_t i = 0; i < length; ++i) {
      m_audioBuffer.push_back(static_cast<T>(data[i] * scale));
    }
    return length;
  }

private:
  std::atomic<bool> m_running;
  std::deque<T> m_audioBuffer;
  std::mutex m_mutex;
  uint32_t m_maxPacketFrames;
};
