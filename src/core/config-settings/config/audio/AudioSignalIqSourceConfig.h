//
// Created by murray on 18/11/25.
//

#pragma once
#include "AudioIqSourceConfig.h"

namespace Config::AudioSignalIqSource
{
  static constexpr auto type = "audiosignaliqsource";

  struct Fields : AudioIqSource::Fields
  {
    bool reverse = false;
  };

  extern Result fromJson(const JsonVariantConst& json, Fields& fields);
}

using AudioSignalIqSourceConfig = Config::AudioSignalIqSource::Fields;
