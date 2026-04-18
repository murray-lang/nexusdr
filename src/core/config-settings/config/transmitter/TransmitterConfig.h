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

  static Result fromJson(JsonVariantConst json, Fields& fields)
  {
    return IqIo::fromJson(json, fields.iqIo);
  }
}