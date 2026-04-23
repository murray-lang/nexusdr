#pragma once
#include "../base/ConfigBase.h"

#include "AudioSignalIqSourceConfig.h"
#include <etl/variant.h>

#include "../base/TypedJson.h"
#include "../control/ControlConfig.h"

namespace Config::IqIo
{
  static constexpr auto type = "iqio";

  using IqSourceVariant = etl::variant<AudioSignalIqSource::Fields>;
  using AudioOutputVariant = etl::variant<Audio::Fields>;

  struct Fields
  {
    IqSourceVariant iqSource;
    AudioOutputVariant audioOutput;
  };

  extern Result fromJson(JsonVariantConst json, Fields& fields);
}

using IqIoConfig = Config::IqIo::Fields;
