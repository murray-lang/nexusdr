//
// Created by murray on 10/08/25.
//

#pragma once


#include <regex>

#include "ResultCode.h"
#include "RtAudioInputDriver.h"
#include "RtAudioOutputDriver.h"

#ifdef USE_ETL
#include <etl/string.h>

using InfoString = etl::string<80>;
#else
using InfoString = std::string;
#endif

class AudioDriverFactory
{
  // static const RtAudio::Api defaultApi = RtAudio::Api::LINUX_ALSA;
public:
  static ResultCode createInputDriver(const Config::Audio::Fields& config, AudioSink* pSink, AudioInputDriver** ppDriver)
  {
    const RtAudio::Api api = apiFromConfig(config);
    if (api == RtAudio::UNSPECIFIED) {
      return ResultCode::ERR_AUDIO_UNKNOWN_API;
    }
    RtAudio::DeviceInfo deviceInfo;
    ResultCode rc = findInputDevice(api, config.searchExpression, &deviceInfo);
    if (rc != ResultCode::OK) return rc;

    AudioDriver::Format format{};
    getInputFormat(config, deviceInfo, format);
    // format.channelCount = pConfig->getChannelCount();
    format.sampleFormat = AUDIO_FLOAT32;
    format.bytesPerFrame = sizeof(float) * format.channelCount;
    *ppDriver = new RtAudioInputDriver(deviceInfo, format, pSink);
    return ResultCode::OK;
  }

  static ResultCode createOutputDriver(const Config::Audio::Fields& config, AudioOutputDriver** ppDriver)
  {
    const RtAudio::Api api = apiFromConfig(config);
    RtAudio::DeviceInfo deviceInfo;
    ResultCode rc = findOutputDevice(api, config.searchExpression, &deviceInfo);
    AudioDriver::Format format{};
    getOutputFormat(config, deviceInfo, format);
    if (format.sampleFormat == AUDIO_FLOAT32) {
      *ppDriver = new RtAudioOutputDriverT<float>(deviceInfo, format);
    } else if (format.sampleFormat == AUDIO_SINT32 || format.sampleFormat == AUDIO_SINT24) {
      *ppDriver = new RtAudioOutputDriverT<int32_t>(deviceInfo, format);
    } else if (format.sampleFormat == AUDIO_SINT16) {
      *ppDriver = new RtAudioOutputDriverT<int16_t>(deviceInfo, format);
    } else if (format.sampleFormat == AUDIO_SINT8) {
      *ppDriver = new RtAudioOutputDriverT<int8_t>(deviceInfo, format);
    } else {
      return ResultCode::ERR_AUDIO_UNKNOWN_FORMAT;
    }
    return ResultCode::OK;
  }

  static RtAudio::Api apiFromConfig(const Config::Audio::Fields& config)
  {
    return apiFromConfig(config.soundApi);
  }

  static RtAudio::Api apiFromConfig(const Config::Audio::ShortString& configApi)
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
    return RtAudio::Api::UNSPECIFIED;
  }

  static ResultCode findInputDevice(
    RtAudio::Api api,
    const Config::Audio::LongString& searchExpression,
    RtAudio::DeviceInfo* pDeviceInfo
  )
  {
    RtAudio audio(api);
    auto deviceIds = audio.getDeviceIds();
    if (deviceIds.empty())
    {
      return ResultCode::ERR_AUDIO_NO_INPUT_DEVICES;
    }
    for (auto& deviceId : deviceIds) {
      RtAudio::DeviceInfo info = audio.getDeviceInfo(deviceId);
      // Only consider devices with input channels
      if (info.inputChannels > 0) {
        InfoString name(info.name.c_str());
        // qDebug() << "Found input device: " << QString::fromStdString(name);
        size_t pos = name.find(searchExpression);
        if (pos != InfoString::npos) {
          *pDeviceInfo = info;
          return ResultCode::OK;
        }
      }
    }
    return ResultCode::ERR_AUDIO_NO_MATCHING_INPUT_DEVICE;
  }

  static ResultCode findOutputDevice(
    RtAudio::Api api,
    const Config::Audio::LongString& searchExpression,
    RtAudio::DeviceInfo* pDeviceInfo
    )
  {
    RtAudio audio(api);
    auto deviceIds = audio.getDeviceIds();
    if (deviceIds.empty())
    {
      return ResultCode::ERR_AUDIO_NO_OUTPUT_DEVICES;
    }
    for (auto& deviceId : deviceIds) {
      RtAudio::DeviceInfo info = audio.getDeviceInfo(deviceId);
      // Only consider devices with input channels
      if (info.outputChannels > 0) {
        InfoString name(info.name.c_str());
        size_t pos = name.find(searchExpression);
        if (pos != InfoString::npos) {
          *pDeviceInfo = info;
          return ResultCode::OK;
        }
      }
    }
    return ResultCode::ERR_AUDIO_NO_MATCHING_OUTPUT_DEVICE;
  }

  static ResultCode findDefaultInputDevice(RtAudio::Api api, RtAudio::DeviceInfo* pDeviceInfo)
  {
    RtAudio audio(api);
    auto deviceIds = audio.getDeviceIds();
    if (deviceIds.empty())
    {
      return ResultCode::ERR_AUDIO_NO_INPUT_DEVICES;
    }
    for (auto& deviceId : deviceIds) {
      RtAudio::DeviceInfo info = audio.getDeviceInfo(deviceId);
      if (info.isDefaultInput) {
        *pDeviceInfo = info;
        return ResultCode::OK;
      }
    }
    return ResultCode::ERR_AUDIO_NO_DEFAULT_INPUT_DEVICE;
  }

  static ResultCode findDefaultOutputDevice(RtAudio::Api api, RtAudio::DeviceInfo* pDeviceInfo)
  {
    RtAudio audio(api);
    auto deviceIds = audio.getDeviceIds();
    if (deviceIds.empty())
    {
      return ResultCode::ERR_AUDIO_NO_OUTPUT_DEVICES;
    }
    for (auto& deviceId : deviceIds) {
      RtAudio::DeviceInfo info = audio.getDeviceInfo(deviceId);
      if (info.isDefaultOutput) {
        *pDeviceInfo = info;
        return ResultCode::OK;
      }
    }
    return ResultCode::ERR_AUDIO_NO_DEFAULT_OUTPUT_DEVICE;
  }

  static void getInputFormat(const Config::Audio::Fields& config, const RtAudio::DeviceInfo& deviceInfo, AudioDriver::Format& format)
  {
    format.sampleFormat = getRtAudioFormatFromConfigOrDefault(config, deviceInfo);
    if (config.sampleRate == 0) {
      format.sampleRate = deviceInfo.preferredSampleRate;
    } else {
      format.sampleRate = static_cast<int>(config.sampleRate);
    }
    if (config.channelCount == 0) {
      format.channelCount = deviceInfo.inputChannels;
    } else {
      format.channelCount = static_cast<int>(config.channelCount);
    }
    format.bytesPerFrame = getBytesPerChannel(format.sampleFormat) * format.channelCount;
  }

  static void getOutputFormat(const Config::Audio::Fields& config, const RtAudio::DeviceInfo& deviceInfo, AudioDriver::Format& format)
  {
    format.sampleFormat = getRtAudioFormatFromConfigOrDefault(config, deviceInfo);
    if (config.sampleRate == 0) {
      format.sampleRate = deviceInfo.preferredSampleRate;
    } else {
      format.sampleRate = static_cast<int>(config.sampleRate);
    }
    if (config.channelCount == 0) {
      format.channelCount = deviceInfo.outputChannels;
    } else {
      format.channelCount = static_cast<int>(config.channelCount);
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

  static RtAudioFormat getRtAudioFormatFromConfigOrDefault(const Config::Audio::Fields& config, const RtAudio::DeviceInfo& deviceInfo)
  {
    RtAudioFormat format = getRtAudioFormatFromConfig(config);
    if (format == 0) {
      format = getDefaultRtAudioFormat(deviceInfo);
    } //else {
      // if (!isRtAudioFormatSupported(format, deviceInfo)) {
      //   std::ostringstream stringStream;
      //   stringStream << "The format '" << pConfig->getFormat() << "' is not supported by the device '" << deviceInfo.name << "'";
      //   std::string copyOfStr = stringStream.str();
      //   throw AudioException(copyOfStr);
      // }
    // }
    return format;
  }

  static RtAudioFormat getRtAudioFormatFromConfig(const Config::Audio::Fields& config)
  {
    return getRtAudioFormatFromConfig(config.format);
  }
  static RtAudioFormat getRtAudioFormatFromConfig(const Config::Audio::ShortString& configFormat)
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

