//
// Created by murray on 18/11/25.
//

#pragma once

#include "core/dsp/iq/AudioIqSource.h"

namespace Config::AudioSignalIqSource
{
  static constexpr auto type = "audiosignaliqsource";

  struct Fields : AudioIqSource::Fields
  {
    bool reverse = false;
  };

  static Result fromJson(const JsonVariantConst& json, Fields& fields)
  {
    Result result = AudioIqSource::fromJson(json, fields);
    fields.type = type; // Override what was set above
    if (result != Result::OK) return result;
    fields.reverse = json["reverse"] ? json["reverse"].as<bool>() : false;
    return Result::OK;
  }
}
