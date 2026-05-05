#pragma once
#include "../base/ConfigBase.h"

#include "AudioSignalIqSourceConfig.h"
#include <etl/variant.h>

#include "../base/TypedJson.h"
#include "../control/ControlConfig.h"

namespace Config::IqIo
{
  static constexpr auto type = "iqio";

  using IqSourceConfigVariant = etl::variant<AudioSignalIqSource::Fields, AudioIqSource::Fields>;
  using AudioOutputConfigVariant = etl::variant<Audio::Fields>;

  struct Fields
  {
    IqSourceConfigVariant iqSource;
    AudioOutputConfigVariant audioOutput;
  };

  extern ResultCode fromJson(JsonVariantConst json, Fields& fields);
}

// using IqIoConfig = Config::IqIo::Fields;
