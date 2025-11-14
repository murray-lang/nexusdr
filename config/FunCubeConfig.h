//
// Created by murray on 15/9/25.
//

#ifndef CUTESDR_VK6HL_FUNCUBECONFIG_H
#define CUTESDR_VK6HL_FUNCUBECONFIG_H
#include "ConfigBase.h"

class FunCubeConfig : public ConfigBase
{
public:
  static constexpr auto type = "funcube";

  FunCubeConfig() : ConfigBase(type) {}
  ~FunCubeConfig() override = default;

  void fromJson(const nlohmann::json& json) override {}

};
#endif //CUTESDR_VK6HL_FUNCUBECONFIG_H