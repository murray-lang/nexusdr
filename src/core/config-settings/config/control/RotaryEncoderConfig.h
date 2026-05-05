//
// Created by murray on 15/9/25.
//

#pragma once

#include "DigitalInputConfig.h"

namespace Config::RotaryEncoder
{
  static constexpr auto type = "rotaryencoder";

  struct Fields : DigitalInput::Fields {};

  extern ResultCode fromJson(JsonVariantConst json, Fields& fields);
}

// using RotaryEncoderConfig = Config::RotaryEncoder::Fields;

