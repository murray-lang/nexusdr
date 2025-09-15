//
// Created by murray on 10/08/25.
//

#ifndef AUDIODEVICEFACTORY_H
#define AUDIODEVICEFACTORY_H
#include "IqAudioInputDevice.h"

class AudioDeviceFactory
{
  // static const RtAudio::Api defaultApi = RtAudio::Api::LINUX_ALSA;
public:
  static IqAudioInputDevice * createInputDevice(const AudioConfig* pConfig, IqSink* pSink)
  {
    const RtAudio::Api api = apiFromConfig(pConfig);
    const RtAudio::DeviceInfo deviceInfo = findInputDevice(api, pConfig->getSearchExpression());
    AudioDevice::Format format{};
    getInputFormat(pConfig, deviceInfo, format);
    format.sampleFormat = RTAUDIO_FLOAT32;
    format.bytesPerFrame = 2 * sizeof(float);
    return new IqAudioInputDevice(deviceInfo, format, pSink);

  }

  static AudioOutputDevice * createOutputDevice(const AudioConfig* pConfig)
  {
    const RtAudio::Api api = apiFromConfig(pConfig);
    const RtAudio::DeviceInfo deviceInfo = findOutputDevice(api, pConfig->getSearchExpression());
    AudioDevice::Format format{};
    getOutputFormat(pConfig, deviceInfo, format);
    return new AudioOutputDevice(deviceInfo, format);
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

  static void getInputFormat(const AudioConfig* pConfig, const RtAudio::DeviceInfo& deviceInfo, AudioDevice::Format& format)
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

  static void getOutputFormat(const AudioConfig* pConfig, const RtAudio::DeviceInfo& deviceInfo, AudioDevice::Format& format)
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

  static uint32_t getBytesPerChannel(RtAudioFormat rtFormat)
  {
    switch (rtFormat)
    {
    case RTAUDIO_SINT8:
      return 1;
    case RTAUDIO_SINT16:
      return 2;
    case RTAUDIO_FLOAT32:
      return 4;
    case RTAUDIO_FLOAT64:
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
    if (deviceInfo.nativeFormats & RTAUDIO_FLOAT32) {
      return RTAUDIO_FLOAT32;
    } else if (deviceInfo.nativeFormats & RTAUDIO_SINT32) {
      return RTAUDIO_SINT32;
    } else if (deviceInfo.nativeFormats & RTAUDIO_SINT24) {
      return RTAUDIO_SINT24;
    } else if (deviceInfo.nativeFormats & RTAUDIO_SINT16) {
      return RTAUDIO_SINT16;
    } else if (deviceInfo.nativeFormats & RTAUDIO_SINT8) {
      return RTAUDIO_SINT8;
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
      return RTAUDIO_FLOAT32;
    } else if (configFormat == "float64") {
      return RTAUDIO_FLOAT64;
    } else if (configFormat == "sint8") {
      return RTAUDIO_SINT8;
    } else if (configFormat == "sint16") {
      return RTAUDIO_SINT16;
    } else if (configFormat == "sint24") {
      return RTAUDIO_SINT24;
    } else if (configFormat == "sint32") {
      return RTAUDIO_SINT32;
    }
    return 0;
  }
};

#endif //AUDIODEVICEFACTORY_H
