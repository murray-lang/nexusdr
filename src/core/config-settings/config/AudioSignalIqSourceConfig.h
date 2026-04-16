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
  static constexpr auto type = "audiosignaliqsource";
  explicit AudioSignalIqSourceConfig() : ConfigBase(type) {}
  AudioSignalIqSourceConfig(const AudioSignalIqSourceConfig& rhs) = default;
  ~AudioSignalIqSourceConfig() override = default;
  AudioSignalIqSourceConfig& operator=(const AudioSignalIqSourceConfig& rhs) = default;

  [[nodiscard]] const AudioConfig& getAudioInput() const { return audioInput; }
  [[nodiscard]] bool getReverse() const { return reverse; }

  void fromJson(JsonVariantConst json) override
  {
    if (json["audioInput"]) {
      audioInput.fromJson(json["audioInput"]);
    }
    if (json["reverse"]) {
      reverse = json["reverse"];
    }
  }

  [[nodiscard]] AudioSignalIqSourceConfig getFields() const
  {
    return static_cast<const AudioSignalIqSourceConfig&>(*this);
  }

  void toJson(JsonObject& json) const override
  {
    JsonObject audioInputObj = json["audioInput"].to<JsonObject>();
    audioInput.toJson(audioInputObj);
    json["reverse"] = reverse;
  }


};