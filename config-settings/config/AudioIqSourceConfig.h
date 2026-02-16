//
// Created by murray on 17/11/25.
//

#pragma once
#include "AudioConfig.h"
#include "ConfigBase.h"

struct AudioIqSourceConfigFields
{
  AudioConfig audioInput;
};

class AudioIqSourceConfig : public ConfigBase, public AudioIqSourceConfigFields
{
public:
  static constexpr auto type = "audioiqsource";
  explicit AudioIqSourceConfig() : ConfigBase(type) {}
  AudioIqSourceConfig(const AudioIqSourceConfig& rhs) = default;
  ~AudioIqSourceConfig() override = default;
  AudioIqSourceConfig& operator=(const AudioIqSourceConfig& rhs) = default;
  [[nodiscard]] const AudioConfig& getAudioInput() const { return audioInput; }

  void fromJson(const nlohmann::json& json) override
  {
    if (json.contains("audioInput")) {
      audioInput.fromJson(json["audioInput"]);
    }
  }

  [[nodiscard]] AudioIqSourceConfigFields getFields() const
  {
    return static_cast<const AudioIqSourceConfigFields&>(*this);
  }

  [[nodiscard]] nlohmann::json toJson() const override
  {
    return nlohmann::json{
      {"audioInput", audioInput.toJson()}
    };
  }
};
