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

  extern Result fromJson(JsonVariantConst json, Fields& fields);
}

// using FunCubeConfig = Config::FunCube::Fields;
