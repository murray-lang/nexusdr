//
// Created by murray on 27/07/25.
//

#ifndef CUTESDR_VK6HL_AUDIOCONFIG_H
#define CUTESDR_VK6HL_AUDIOCONFIG_H
#include "./JsonConfig.h"

class AudioConfig : public JsonConfig
{
  friend class TransmitterConfig;
  friend class ReceiverConfig;
public:
  explicit AudioConfig() :
    m_isInput(false),
    m_isIq(false),
    m_sampleRate(0),
    m_channelCount(0)
  {
  }

  AudioConfig(const AudioConfig& rhs) :
    m_isInput(false),
    m_isIq(rhs.m_isIq),
    m_soundApi(rhs.m_soundApi),
    m_searchExpression(rhs.m_searchExpression),
    m_sampleRate(rhs.m_sampleRate),
    m_format(rhs.m_format),
    m_channelCount(rhs.m_channelCount)
  {
//    operator=(rhs);
  }

  AudioConfig& operator=(const AudioConfig& rhs)
  {
    m_isInput = rhs.m_isInput;
    m_isIq = rhs.m_isIq;
    m_soundApi = rhs.m_soundApi;
    m_searchExpression = rhs.m_searchExpression;
    m_sampleRate = rhs.m_sampleRate;
    m_format = rhs.m_format;
    m_channelCount = rhs.m_channelCount;
    return *this;
  }

  static AudioConfig fromJson(const nlohmann::json& json)
  {
    AudioConfig result;
    if (json.contains("soundApi")) {
      result.m_soundApi = json["soundApi"].get<std::string>();
    }
    if (json.contains("isInput")) {
      result.m_isIq = json["isInput"].get<bool>();
    }
    if (json.contains("isIq")) {
      result.m_isIq = json["isIq"].get<bool>();
    }
    if (json.contains("searchExpression")) {
      result.m_searchExpression = json["searchExpression"].get<std::string>();
    }
    if (json.contains("sampleRate")) {
      result.m_sampleRate = json["sampleRate"].get<uint32_t>();
    }
    if (json.contains("channelCount")) {
      result.m_channelCount = json["channelCount"].get<uint32_t>();
    }
    if (json.contains("format")) {
      result.m_format = json["format"].get<std::string>();
    }
    return result;
  }

  [[nodiscard]] bool isIq() const { return m_isIq; }
  [[nodiscard]] const std::string& getSearchExpression() const { return m_searchExpression; }
  [[nodiscard]] uint32_t getSampleRate() const { return m_sampleRate; }
  [[nodiscard]] uint32_t getChannelCount() const { return m_channelCount; }
  [[nodiscard]] const std::string& getFormat() const { return m_format; }
  [[nodiscard]] const std::string& getSoundApi() const { return m_soundApi; }

protected:
  bool m_isInput;
  bool m_isIq;
  std::string m_searchExpression;
  std::string m_soundApi; // "alsa", "pulse", "jack", "oss", "dummy"
  std::string m_format; // "sint8", "sint16", "sint24", "sint32", "float32", "float64"
  uint32_t m_sampleRate;
  uint32_t m_channelCount;
};
#endif //CUTESDR_VK6HL_AUDIOCONFIG_H
