//
// Created by murray on 14/9/25.
//

#pragma once
#include "ConfigBase.h"
#include "VariantConfig.h"


class ConfigFactory
{
public:
  static ConfigBase* create(const VariantConfig& item);
  static ConfigBase* create(const std::string& type);
  static ConfigBase* create(const std::string& type, JsonVariantConst config);
};
