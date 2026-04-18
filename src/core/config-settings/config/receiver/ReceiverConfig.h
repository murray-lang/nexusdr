//
// Created by murray on 27/07/25.
//

#pragma once

#include "../base/ConfigBase.h"
#include "../audio/IqIoConfig.h"

namespace Config::Receiver
{
  static constexpr auto type = "receiver";

  struct Fields
  {
    IqIo::Fields iqIo;
  };

  static Result fromJson(JsonVariantConst json, Fields& fields)
  {
    return IqIo::fromJson(json, fields.iqIo);
  }
}
