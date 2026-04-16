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

  void fromJson(JsonVariantConst json) override
  {
    if (json["audioInput"]) {
      audioInput.fromJson(json["audioInput"]);
    }
  }

  [[nodiscard]] AudioIqSourceConfigFields getFields() const
  {
    return static_cast<const AudioIqSourceConfigFields&>(*this);
  }

  void toJson(JsonObject& json) const override
  {
    JsonObject audioInputObj = json["audioInput"].to<JsonObject>();
    audioInput.toJson(audioInputObj);
  }
};
