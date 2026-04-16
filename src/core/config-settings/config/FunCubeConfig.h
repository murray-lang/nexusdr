//
// Created by murray on 15/9/25.
//

#pragma once
#include "ConfigBase.h"

class FunCubeConfig : public ConfigBase
{
public:
  static constexpr auto type = "funcube";

  FunCubeConfig() : ConfigBase(type) {}
  ~FunCubeConfig() override = default;

  void fromJson(JsonVariantConst json) override {}

};
