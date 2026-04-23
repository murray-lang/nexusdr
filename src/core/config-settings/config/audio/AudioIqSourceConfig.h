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

  extern Result fromJson(const JsonVariantConst& json, Fields& fields);
}

using AudioIqSourceConfig = Config::AudioIqSource::Fields;
