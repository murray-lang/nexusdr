//
// Created by murray on 4/08/25.
//

#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H
// #include <QAudioSource>
// #include <QMediaDevices>
// #include <QAudioFormat>
// #include <QIODevice>
#include <rtaudio/RtAudio.h>
#include <string>
#include <algorithm>
#include <optional>

#include <regex>
#include "../../../radio/config/AudioConfig.h"
#include "../AudioException.h"


class AudioDevice
{
protected:
  // Q_OBJECT
  virtual ~AudioDevice() = default;

public:
  struct Format
  {
    uint32_t sampleRate;
    uint32_t channelCount;
    uint32_t bytesPerFrame;
    RtAudioFormat sampleFormat;
  };

  explicit AudioDevice(const RtAudio::DeviceInfo& deviceInfo, const Format& format) :
    m_format(format),
    m_deviceInfo(deviceInfo)
  {
  }

  virtual void start() = 0;
  virtual void stop() = 0;

protected:
  RtAudio m_rtAudio;
  Format m_format;
  RtAudio::DeviceInfo m_deviceInfo;
};

#endif //AUDIODEVICE_H
