//
// Created by murray on 6/08/25.
//
#include "SinkControllerFactory.h"
#include <algorithm>
#include <cctype>
#include <nlohmann/json.hpp>

#include "device/FunCubeDongle/FunCubeDongle.h"

SinkController*
SinkControllerFactory::create(const ControllerConfig& config)
{
  SinkController* result = create(config.getType());
  if (result)
  {
    result->setId(config.getType());
    result->initialise(config.getConfig());
  }
  return result;
}

SinkController*
SinkControllerFactory::create(const std::string& type)
{
  std::string typeAslower = toLowerCase(type);
  if(typeAslower == "funcube")
  {
    return new FunCubeDongle();
  }
  return nullptr;
}

std::string
SinkControllerFactory::toLowerCase(const std::string& str)
{
  std::string output = str;
  std::transform(output.begin(), output.end(), output.begin(),
                 [](unsigned char c){ return std::tolower(c); });
  return output;

}