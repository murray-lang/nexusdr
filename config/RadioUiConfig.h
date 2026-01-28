//
// Created by murray on 28/1/26.
//

#pragma once
#include "ConfigBase.h"

struct RadioUiConfigFields
{
  std::string face;
};

class RadioUiConfig : public ConfigBase, public RadioUiConfigFields
{
  public:
  static constexpr auto type = "radioui";

  RadioUiConfig() : ConfigBase(type) {}
  ~RadioUiConfig() override = default;
  void fromJson(const nlohmann::json& json) override
  {
    if (json.contains("face")) {
      face = json["face"];
    }
  }

  [[nodiscard]] nlohmann::json toJson() const override
  {
    return nlohmann::json{{"face", face}};
  }

};