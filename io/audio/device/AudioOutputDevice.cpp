//
// Created by murray on 18/07/25.
//

#include "AudioOutputDevice.h"
// #include <qdebug.h>


AudioOutputDevice::AudioOutputDevice(const RtAudio::DeviceInfo& deviceInfo, const Format& format) :
  AudioDevice(deviceInfo, format),
  m_running(false),
  m_audioBuffer(),
  m_maxPacketFrames(0)
{
}

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
  static size_t numUnderruns = 0;
  auto* out = static_cast<int16_t*>(outputBuffer);
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
      sdrreal scaled = value * static_cast<int16_t>(INT16_MAX);
      return static_cast<int16_t>(scaled);
      // return static_cast<float>(value);
    }
  );
  return length;
}