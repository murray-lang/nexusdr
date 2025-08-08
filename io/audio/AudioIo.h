//
// Created by murray on 5/08/25.
//

#ifndef AUDIOIO_H
#define AUDIOIO_H
#include "device/AudioDevice.h"
#include "../../radio/config/AudioConfig.h"

class AudioIo
{
public:

  AudioIo() = default;

  virtual ~AudioIo() = default;

  virtual void start() const = 0;
  virtual void stop() const = 0;

  void configure(const AudioConfig& config)
  {
    m_config = config;
    const std::string& searchExpression = config.getSearchExpression();

    if (searchExpression.empty())
    {
      m_deviceInfo = getDefaultDevice();
    } else
    {
      m_deviceInfo = findDevice(searchExpression);
    }
    if (config.getSampleRate() == 0)
    {
      m_format.sampleRate = m_deviceInfo.preferredSampleRate;
    } else
    {
      m_format.sampleRate = static_cast<int>(config.getSampleRate());
    }
    if (config.getChannelCount() == 0)
    {
      m_format.channelCount = m_deviceInfo.outputChannels;
    } else
    {
      m_format.channelCount = static_cast<int>(config.getChannelCount());
    }
    m_format.sampleFormat = RTAUDIO_FLOAT32;
    m_format.bytesPerFrame = sizeof(float) * m_format.channelCount;
  };

protected:
  virtual RtAudio::DeviceInfo findDevice(const std::string& searchExpression) = 0;
  virtual RtAudio::DeviceInfo getDefaultDevice() = 0;

protected:
  AudioConfig m_config;
  RtAudio::DeviceInfo m_deviceInfo;
  AudioDevice::Format m_format;
};

#endif //AUDIOIO_H
