//
// Created by murray on 18/07/25.
//

#include "AudioOutputDevice.h"


AudioOutputDevice::AudioOutputDevice(const RtAudio::DeviceInfo& deviceInfo, const Format& format) :
  AudioDevice(deviceInfo, format),
  m_running(false),
  m_audioBuffer()
{
}

void AudioOutputDevice::start()
{
  if (!m_running) {
    m_running = true;
    RtAudio::StreamParameters parameters;
    parameters.deviceId = m_deviceInfo.ID;
    parameters.nChannels = std::min(m_deviceInfo.outputChannels, static_cast<unsigned int>(2));
    parameters.firstChannel = 0;

    RtAudio::StreamOptions options{
      .flags = RTAUDIO_NONINTERLEAVED,
      .numberOfBuffers = 2
    };
    // Set options as needed

    unsigned int sampleRate = m_format.sampleRate;
    unsigned int bufferFrames = 512;

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
      &bufferFrames, rtCallback, this, &options);

    rc = m_rtAudio.startStream();
  }

}

void AudioOutputDevice::stop()
{
  if (m_running) {
    if (m_rtAudio.isStreamOpen()) {
      m_rtAudio.stopStream();
      m_rtAudio.closeStream();
      m_running = false;
    }
  }
}

int AudioOutputDevice::pullSamples(void *outputBuffer, unsigned int nFrames)
{
  sdrreal* out = static_cast<sdrreal*>(outputBuffer);
  std::lock_guard<std::mutex> lock(m_mutex);
  for (unsigned int i = 0; i < nFrames; ++i) {
    if (!m_audioBuffer.empty()) {
      out[i] = m_audioBuffer.front();
      m_audioBuffer.pop_front();
    } else {
      out[i] = 0.0f; // silence if no data
    }
  }
  return 0; // 0: continue, nonzero: stop
}



uint32_t
AudioOutputDevice::addAudioData(const vsdrreal& data, const uint32_t length)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  std::transform(
    data.begin(),
    data.begin() + length,
    std::back_inserter(m_audioBuffer),
    [](const sdrreal& value)
    {
      return static_cast<float>(value);
    }
  );
  return length;
}