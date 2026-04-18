//
// Created by murray on 15/9/25.
//

#pragma once
#include "../base/ConfigBase.h"

namespace Config::FunCube
{
  static constexpr auto type = "funcube";

  struct Fields : Alternative
  {
    // TODO!!
  };

  static Result fromJson(JsonVariantConst json, Fields& fields)
  {
    fields.type = type;
    return Result::OK;
  }
}
