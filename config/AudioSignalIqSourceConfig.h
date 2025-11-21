//
// Created by murray on 18/11/25.
//

#pragma once

#include "AudioConfig.h"
#include "ConfigBase.h"

struct AudioSignalIqSourceConfigFields
{
  AudioConfig audioInput;
  bool reverse = false;
};

class AudioSignalIqSourceConfig : public ConfigBase, public AudioSignalIqSourceConfigFields
{
public:
  static constexpr auto type = "audioSignalIqSource";
  explicit AudioSignalIqSourceConfig() : ConfigBase(type) {}
  AudioSignalIqSourceConfig(const AudioSignalIqSourceConfig& rhs) = default;
  ~AudioSignalIqSourceConfig() override = default;
  AudioSignalIqSourceConfig& operator=(const AudioSignalIqSourceConfig& rhs) = default;

  [[nodiscard]] const AudioConfig& getAudioInput() const { return audioInput; }
  [[nodiscard]] bool getReverse() const { return reverse; }

  void fromJson(const nlohmann::json& json) override
  {
    if (json.contains("audioInput")) {
      audioInput.fromJson(json["audioInput"]);
    }
    if (json.contains("reverse")) {
      reverse = json["reverse"];
    }
  }

  [[nodiscard]] AudioSignalIqSourceConfig getFields() const
  {
    return static_cast<const AudioSignalIqSourceConfig&>(*this);
  }

  [[nodiscard]] nlohmann::json toJson() const override
  {
    return nlohmann::json{
  {"audioInput", audioInput.toJson()},
  {"reverse", reverse }
    };
  }


};