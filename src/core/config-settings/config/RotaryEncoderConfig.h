//
// Created by murray on 15/9/25.
//

#pragma once

#include "DigitalInputConfig.h"

class RotaryEncoderConfig : public DigitalInputConfig
{
public:
  static constexpr auto type = "rotaryencoder";

  RotaryEncoderConfig() : DigitalInputConfig(type) {}
  ~RotaryEncoderConfig() override = default;

  void fromJson(JsonVariantConst json) override
  {
    DigitalInputConfig::fromJson(json);
  }
};
