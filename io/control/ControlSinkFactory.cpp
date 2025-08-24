//
// Created by murray on 6/08/25.
//
#include "ControlSinkFactory.h"
#include <algorithm>
#include <cctype>
#include <nlohmann/json.hpp>

#include "device/FunCubeDongle/FunCubeDongle.h"

ControlSink*
ControlSinkFactory::create(const ControlBaseConfig& config)
{
  ControlSink* result = create(config.getType());
  if (result)
  {
    result->setId(config.getType());
    result->initialise(config.getConfig());
  }
  return result;
}

ControlSink*
ControlSinkFactory::create(const std::string& type)
{
  std::string typeAslower = StringUtils::toLowerCase(type);
  if(typeAslower == "funcube")
  {
    return new FunCubeDongle();
  }
  return nullptr;
}

// std::string
// ControlSinkFactory::toLowerCase(const std::string& str)
// {
//   std::string output = str;
//   std::transform(output.begin(), output.end(), output.begin(),
//                  [](unsigned char c){ return std::tolower(c); });
//   return output;
//
// }