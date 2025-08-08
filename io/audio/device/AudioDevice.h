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
#include "../AudioException.h"


class AudioDevice
{
protected:
  // Q_OBJECT
  ~AudioDevice() = default;

public:
  struct Format
  {
    int sampleRate;
    int channelCount;
    int bytesPerFrame;
    RtAudioFormat sampleFormat;
  };

  explicit AudioDevice(const RtAudio::DeviceInfo& deviceInfo, const Format& format) :
    m_format(format),
    m_deviceInfo(deviceInfo)
  {
  }

  static RtAudio::DeviceInfo findInputDevice(const std::string& searchExpression)
  {
    RtAudio audio(RtAudio::Api::LINUX_ALSA);
    auto deviceIds = audio.getDeviceIds();
    if (deviceIds.size() == 0)
    {
      throw AudioException("No audio input devices were found");
    }
    std::basic_regex<char> regex(searchExpression, std::regex_constants::ECMAScript | std::regex_constants::icase);
    for (auto& deviceId : deviceIds) {
      RtAudio::DeviceInfo info = audio.getDeviceInfo(deviceId);
      // Only consider devices with input channels
      if (info.inputChannels > 0) {
        std::string name = info.name;
        std::smatch match;
        std::regex_search(name, match, regex);
        if (std::regex_search(name, match, regex)) {
          return info;
        }
      }
    }
    std::ostringstream stringStream;
    stringStream << "An audio device with a name matched by '" << searchExpression << "' was not found";
    std::string copyOfStr = stringStream.str();
    throw AudioException(copyOfStr);
  }

  static RtAudio::DeviceInfo findOutputDevice(const std::string& searchExpression)
  {
    RtAudio audio(RtAudio::Api::LINUX_ALSA);
     auto deviceIds = audio.getDeviceIds();
    if (deviceIds.size() == 0)
    {
      throw AudioException("No audio input devices were found");
    }
    std::basic_regex<char> regex(searchExpression, std::regex_constants::ECMAScript | std::regex_constants::icase);
    for (auto& deviceId : deviceIds) {
      RtAudio::DeviceInfo info = audio.getDeviceInfo(deviceId);
      // Only consider devices with input channels
      if (info.outputChannels > 0) {
        std::string name = info.name;
        std::smatch match;
        std::regex_search(name, match, regex);
        if (std::regex_search(name, match, regex)) {
          return info;
        }
      }
    }
    std::ostringstream stringStream;
    stringStream << "An audio device with a name matched by '" << searchExpression << "' was not found";
    std::string copyOfStr = stringStream.str();
    throw AudioException(copyOfStr);
  }

  static RtAudio::DeviceInfo findDefaultInputDevice()
  {
    RtAudio audio(RtAudio::Api::LINUX_ALSA);
    auto deviceIds = audio.getDeviceIds();
    if (deviceIds.size() == 0)
    {
      throw AudioException("No audio input devices were found");
    }
    for (auto& deviceId : deviceIds) {
      RtAudio::DeviceInfo info = audio.getDeviceInfo(deviceId);
      if (info.isDefaultInput) {
        return info;
      }
    }
    throw AudioException("No default audio input device found");
  }

  static RtAudio::DeviceInfo findDefaultOutputDevice()
  {
    RtAudio audio(RtAudio::Api::LINUX_ALSA);
    auto deviceIds = audio.getDeviceIds();
    if (deviceIds.size() == 0)
    {
      throw AudioException("No audio output devices were found");
    }
    for (auto& deviceId : deviceIds) {
      RtAudio::DeviceInfo info = audio.getDeviceInfo(deviceId);
      if (info.isDefaultOutput) {
        return info;
      }
    }
    throw AudioException("No default audio output device found");
  }

protected:
  RtAudio m_rtAudio;
  Format m_format;
  RtAudio::DeviceInfo m_deviceInfo;
};

#endif //AUDIODEVICE_H
