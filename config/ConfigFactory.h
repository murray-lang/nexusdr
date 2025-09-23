//
// Created by murray on 14/9/25.
//

#ifndef CUTESDR_VK6HL_CONFIGFACTORY_H
#define CUTESDR_VK6HL_CONFIGFACTORY_H
#include "ConfigBase.h"
#include "VariantConfig.h"
#include <nlohmann/json.hpp>


class ConfigFactory
{
public:
  static ConfigBase* create(const VariantConfig& item);
  static ConfigBase* create(const std::string& type);
  static ConfigBase* create(const std::string& type, const nlohmann::json& config);
};


#endif //CUTESDR_VK6HL_CONFIGFACTORY_H