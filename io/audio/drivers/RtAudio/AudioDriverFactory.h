//
// Created by murray on 10/08/25.
//

#pragma once


#include <regex>

#include "RtAudioInputDriver.h"
#include "RtAudioOutputDriver.h"
#include "io/audio/AudioException.h"

class AudioDriverFactory
{
  // static const RtAudio::Api defaultApi = RtAudio::Api::LINUX_ALSA;
public:
  static AudioInputDriver* createInputDriver(const AudioConfig* pConfig, AudioSink* pSink)
  {
    const RtAudio::Api api = apiFromConfig(pConfig);
    const RtAudio::DeviceInfo deviceInfo = findInputDevice(api, pConfig->getSearchExpression());
    AudioDriver::Format format{};
    getInputFormat(pConfig, deviceInfo, format);
    // format.channelCount = pConfig->getChannelCount();
    format.sampleFormat = AUDIO_FLOAT32;
    format.bytesPerFrame = sizeof(float) * format.channelCount;
    return new RtAudioInputDriver(deviceInfo, format, pSink);
  }

  static AudioOutputDriver * createOutputDriver(const AudioConfig* pConfig)
  {
    const RtAudio::Api api = apiFromConfig(pConfig);
    const RtAudio::DeviceInfo deviceInfo = findOutputDevice(api, pConfig->getSearchExpression());
    AudioDriver::Format format{};
    getOutputFormat(pConfig, deviceInfo, format);
    if (format.sampleFormat == AUDIO_FLOAT32) {
      return new RtAudioOutputDriverT<float>(deviceInfo, format);
    } else if (format.sampleFormat == AUDIO_SINT32 || format.sampleFormat == AUDIO_SINT24) {
      return new RtAudioOutputDriverT<int32_t>(deviceInfo, format);
    } else if (format.sampleFormat == AUDIO_SINT16) {
      return new RtAudioOutputDriverT<int16_t>(deviceInfo, format);
    } else if (format.sampleFormat == AUDIO_SINT8) {
      return new RtAudioOutputDriverT<int8_t>(deviceInfo, format);
    }
    return nullptr;
  }

  static RtAudio::Api apiFromConfig(const AudioConfig* pConfig)
  {
    return apiFromConfig(pConfig->getSoundApi());
  }

  static RtAudio::Api apiFromConfig(const std::string& configApi)
  {
    if (configApi == "alsa") {
      return RtAudio::Api::LINUX_ALSA;
    } else if (configApi == "oss") {
      return RtAudio::Api::LINUX_OSS;
    } else if (configApi == "pulse") {
      return RtAudio::Api::LINUX_PULSE;
    } else if (configApi == "jack") {
      return RtAudio::Api::UNIX_JACK;
    }
    std::ostringstream stringStream;
    stringStream << "Sound API '" << configApi << "' is not supported";
    std::string copyOfStr = stringStream.str();
    throw AudioException(copyOfStr);
  }

  static RtAudio::DeviceInfo findInputDevice(RtAudio::Api api, const std::string& searchExpression)
  {
    RtAudio audio(api);
    auto deviceIds = audio.getDeviceIds();
    if (deviceIds.empty())
    {
      throw AudioException("No audio input devices were found");
    }
    std::basic_regex<char> regex(searchExpression, std::regex_constants::ECMAScript | std::regex_constants::icase);
    for (auto& deviceId : deviceIds) {
      RtAudio::DeviceInfo info = audio.getDeviceInfo(deviceId);
      // Only consider devices with input channels
      if (info.inputChannels > 0) {
        std::string name = info.name;
        // qDebug() << "Found input device: " << QString::fromStdString(name);
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

  static RtAudio::DeviceInfo findOutputDevice(RtAudio::Api api, const std::string& searchExpression)
  {
    RtAudio audio(api);
    auto deviceIds = audio.getDeviceIds();
    if (deviceIds.empty())
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

  static RtAudio::DeviceInfo findDefaultInputDevice(RtAudio::Api api)
  {
    RtAudio audio(api);
    auto deviceIds = audio.getDeviceIds();
    if (deviceIds.empty())
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

  static RtAudio::DeviceInfo findDefaultOutputDevice(RtAudio::Api api)
  {
    RtAudio audio(api);
    auto deviceIds = audio.getDeviceIds();
    if (deviceIds.empty())
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

  static void getInputFormat(const AudioConfig* pConfig, const RtAudio::DeviceInfo& deviceInfo, AudioDriver::Format& format)
  {
    format.sampleFormat = getRtAudioFormatFromConfigOrDefault(pConfig, deviceInfo);
    if (pConfig->getSampleRate() == 0) {
      format.sampleRate = deviceInfo.preferredSampleRate;
    } else {
      format.sampleRate = static_cast<int>(pConfig->getSampleRate());
    }
    if (pConfig->getChannelCount() == 0) {
      format.channelCount = deviceInfo.inputChannels;
    } else {
      format.channelCount = static_cast<int>(pConfig->getChannelCount());
    }
    format.bytesPerFrame = getBytesPerChannel(format.sampleFormat) * format.channelCount;
  }

  static void getOutputFormat(const AudioConfig* pConfig, const RtAudio::DeviceInfo& deviceInfo, AudioDriver::Format& format)
  {
    format.sampleFormat = getRtAudioFormatFromConfigOrDefault(pConfig, deviceInfo);
    if (pConfig->getSampleRate() == 0) {
      format.sampleRate = deviceInfo.preferredSampleRate;
    } else {
      format.sampleRate = static_cast<int>(pConfig->getSampleRate());
    }
    if (pConfig->getChannelCount() == 0) {
      format.channelCount = deviceInfo.outputChannels;
    } else {
      format.channelCount = static_cast<int>(pConfig->getChannelCount());
    }
    format.bytesPerFrame = getBytesPerChannel(format.sampleFormat) * format.channelCount;
  }

  static uint32_t getBytesPerChannel(AudioFormat rtFormat)
  {
    switch (rtFormat)
    {
    case AUDIO_SINT8:
      return 1;
    case AUDIO_SINT16:
      return 2;
    case AUDIO_SINT24:
      return 3;
    case AUDIO_SINT32:
    case AUDIO_FLOAT32:
      return 4;
    case AUDIO_FLOAT64:
      return 8;
    default:
      return 0;
    }
  }

  static bool isRtAudioFormatSupported(RtAudioFormat format, const RtAudio::DeviceInfo& deviceInfo)
  {
    return (deviceInfo.nativeFormats & format) != 0;
  }

  static RtAudioFormat getDefaultRtAudioFormat(const RtAudio::DeviceInfo& deviceInfo)
  {
    if (deviceInfo.nativeFormats & AUDIO_FLOAT32) {
      return AUDIO_FLOAT32;
    } else if (deviceInfo.nativeFormats & AUDIO_SINT32) {
      return AUDIO_SINT32;
    } else if (deviceInfo.nativeFormats & AUDIO_SINT24) {
      return AUDIO_SINT24;
    } else if (deviceInfo.nativeFormats & AUDIO_SINT16) {
      return AUDIO_SINT16;
    } else if (deviceInfo.nativeFormats & AUDIO_SINT8) {
      return AUDIO_SINT8;
    }
    return 0;
  }

  static RtAudioFormat getRtAudioFormatFromConfigOrDefault(const AudioConfig* pConfig, const RtAudio::DeviceInfo& deviceInfo)
  {
    RtAudioFormat format = getRtAudioFormatFromConfig(pConfig);
    if (format == 0) {
      format = getDefaultRtAudioFormat(deviceInfo);
    } else {
      if (!isRtAudioFormatSupported(format, deviceInfo)) {
        std::ostringstream stringStream;
        stringStream << "The format '" << pConfig->getFormat() << "' is not supported by the device '" << deviceInfo.name << "'";
        std::string copyOfStr = stringStream.str();
        throw AudioException(copyOfStr);
      }
    }
    return format;
  }

  static RtAudioFormat getRtAudioFormatFromConfig(const AudioConfig* pConfig)
  {
    return getRtAudioFormatFromConfig(pConfig->getFormat());
  }
  static RtAudioFormat getRtAudioFormatFromConfig(const std::string& configFormat)
  {
    if (configFormat == "float32") {
      return AUDIO_FLOAT32;
    } else if (configFormat == "float64") {
      return AUDIO_FLOAT64;
    } else if (configFormat == "sint8") {
      return AUDIO_SINT8;
    } else if (configFormat == "sint16") {
      return AUDIO_SINT16;
    } else if (configFormat == "sint24") {
      return AUDIO_SINT24;
    } else if (configFormat == "sint32") {
      return AUDIO_SINT32;
    }
    return 0;
  }
};

