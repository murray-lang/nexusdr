//
// Created by murray on 28/1/26.
//

#pragma once
#include "ConfigBase.h"

struct UiConfigFields
{
  std::string face;
};

class UiConfig : public ConfigBase, public UiConfigFields
{
  public:
  static constexpr auto type = "ui";

  UiConfig() : ConfigBase(type) {}
  ~UiConfig() override = default;
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