//
// Created by murray on 17/11/25.
//

#pragma once
#include "AudioConfig.h"
#include "../base/TypedJson.h"

namespace Config::AudioIqSource
{
  static constexpr auto type = "audioiqsource";

  struct Fields : Alternative
  {
    Audio::Fields audioInput;
  };

  static Result fromJson(const JsonVariantConst& json, Fields& fields)
  {
    fields.type = type;
    return Audio::fromJson(json, fields.audioInput);
  }
}
