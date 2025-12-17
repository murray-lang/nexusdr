//
// Created by murray on 27/07/25.
//

#pragma once
#include "ConfigBase.h"
#include <string>
#include <cstdint>

// Plain struct containing all audio configuration members for programmer visibility
struct AudioConfigFields
{
  bool isInput = false;
  bool isIq = false;
  std::string soundApi; // "alsa", "pulse", "jack", "oss", "dummy"
  std::string searchExpression;
  uint32_t sampleRate = 0;
  uint32_t channelCount = 0;
  std::string format; // "sint8", "sint16", "sint24", "sint32", "float32", "float64"
};

class AudioConfig : public ConfigBase, public AudioConfigFields
{
public:
  static constexpr auto type = "audio";

  explicit AudioConfig() :
    ConfigBase(type)
  {
  }

  AudioConfig(const AudioConfig& rhs) = default;

  AudioConfig& operator=(const AudioConfig& rhs) = default;
  // {
  //   // Copy the struct portion directly (since we inherit from it)
  //   if (this != &rhs) {
  //     static_cast<AudioConfigFields&>(*this) = static_cast<const AudioConfigFields&>(rhs);
  //   }
  //   return *this;
  // }

  // Populate from a plain Fields struct
  void setFields(const AudioConfigFields& f)
  {
    static_cast<AudioConfigFields&>(*this) = f;
  }

  // Snapshot current state to a plain Fields struct
  [[nodiscard]] AudioConfigFields getFields() const
  {
    return static_cast<const AudioConfigFields&>(*this);
  }
  void fromJson(const nlohmann::json& json) override
  {
    if (json.contains("soundApi")) {
      soundApi = json["soundApi"].get<std::string>();
    }
    if (json.contains("isInput")) {
      isInput = json["isInput"].get<bool>();
    }
    if (json.contains("isIq")) {
      isIq = json["isIq"].get<bool>();
    }
    if (json.contains("searchExpression")) {
      searchExpression = json["searchExpression"].get<std::string>();
    }
    if (json.contains("sampleRate")) {
      sampleRate = json["sampleRate"].get<uint32_t>();
    }
    if (json.contains("channelCount")) {
      channelCount = json["channelCount"].get<uint32_t>();
    }
    if (json.contains("format")) {
      format = json["format"].get<std::string>();
    }
  }

  [[nodiscard]] nlohmann::json toJson() const override
  {
    return nlohmann::json{
      {"isInput", isInput},
      {"isIq", isIq},
      {"soundApi", soundApi},
      {"searchExpression", searchExpression},
      {"sampleRate", sampleRate},
      {"channelCount", channelCount},
      {"format", format}
    };
  }


  [[nodiscard]] bool isIqInput() const { return isIq; }
  [[nodiscard]] const std::string& getSearchExpression() const { return searchExpression; }
  [[nodiscard]] uint32_t getSampleRate() const { return sampleRate; }
  [[nodiscard]] uint32_t getChannelCount() const { return channelCount; }
  [[nodiscard]] const std::string& getFormat() const { return format; }
  [[nodiscard]] const std::string& getSoundApi() const { return soundApi; }
};
