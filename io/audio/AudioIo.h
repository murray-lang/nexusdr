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
      m_format.setSampleRate(m_deviceInfo.preferredFormat().sampleRate());
    } else
    {
      m_format.setSampleRate(static_cast<int>(config.getSampleRate()));
    }
    if (config.getChannelCount() == 0)
    {
      m_format.setChannelCount(m_deviceInfo.preferredFormat().channelCount());
    } else
    {
      m_format.setChannelCount(static_cast<int>(config.getChannelCount()));
    }
    m_format.setSampleFormat(m_deviceInfo.preferredFormat().sampleFormat());
    m_format.setChannelConfig(QAudioFormat::ChannelConfigStereo);
  };

protected:
  virtual QAudioDevice findDevice(const std::string& searchExpression) = 0;
  virtual QAudioDevice getDefaultDevice() = 0;

protected:
  AudioConfig m_config;
  QAudioDevice m_deviceInfo;
  QAudioFormat m_format;
};

#endif //AUDIOIO_H
