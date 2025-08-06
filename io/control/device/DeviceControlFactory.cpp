//
// Created by murray on 6/08/25.
//
#include "DeviceControlFactory.h"
#include <algorithm>
#include <cctype>
#include <nlohmann/json.hpp>

#include "FunCubeDongle/FunCubeDongle.h"

DeviceControl*
DeviceControlFactory::create(const ControlConfig& config)
{
  DeviceControl* result = create(config.getType());
  if (result)
  {
    result->setId(config.getType());
    result->initialise(config.getConfig());
  }
  return result;
}

DeviceControl*
DeviceControlFactory::create(const std::string& type)
{
  std::string typeAslower = toLowerCase(type);
  if(typeAslower == "funcube")
  {
    return new FunCubeDongle();
  }
  return nullptr;
}

std::string
DeviceControlFactory::toLowerCase(const std::string& str)
{
  std::string output = str;
  std::transform(output.begin(), output.end(), output.begin(),
                 [](unsigned char c){ return std::tolower(c); });
  return output;

}