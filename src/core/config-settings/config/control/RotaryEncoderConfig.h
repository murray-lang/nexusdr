//
// Created by murray on 15/9/25.
//

#pragma once

#include "DigitalInputConfig.h"

namespace Config::RotaryEncoder
{
  static constexpr auto type = "rotaryencoder";

  struct Fields : DigitalInput::Fields {};

  inline Result fromJson(JsonVariantConst json, Fields& fields)
  {
    Result result = DigitalInput::fromJson(json, fields);
    fields.type = type;
    return result;
  }
}

