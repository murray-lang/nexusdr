#pragma once

#include "../base/ConfigBase.h"
#include "../audio/IqIoConfig.h"

namespace Config::Transmitter
{
  static constexpr auto type = "transmitter";

  struct Fields
  {
    IqIo::Fields iqIo;
  };

  extern Result fromJson(JsonVariantConst json, Fields& fields);
}

using TransmitterConfig = Config::Transmitter::Fields;