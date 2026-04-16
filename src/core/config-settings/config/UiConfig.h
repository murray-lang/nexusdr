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
  void fromJson(JsonVariantConst json) override
  {
    if (json["face"]) {
      face = json["face"].as<const char *>();
    }
  }

  void toJson(JsonObject& json) const override
  {
    json["face"] = face;
  }

};