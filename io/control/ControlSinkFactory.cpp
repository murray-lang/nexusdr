//
// Created by murray on 6/08/25.
//
#include "ControlSinkFactory.h"
#include <nlohmann/json.hpp>

#include "config/FunCubeConfig.h"
#include "device/FunCubeDongle/FunCubeDongle.h"
#include "device/gpio/digital/DigitalOutput.h"

ControlSink*
ControlSinkFactory::create(const ConfigBase* pConfig)
{
  ControlSink* result = create(pConfig->getType());
  if (result)
  {
    result->setId(pConfig->getType());
    result->configure(pConfig);
  }
  return result;
}

ControlSink*
ControlSinkFactory::create(const std::string& type)
{
  std::string typeAslower = StringUtils::toLowerCase(type);
  if(typeAslower == FunCubeConfig::type)
  {
    return new FunCubeDongle();
  }
  if(typeAslower == DigitalOutputConfig::type)
  {
    return new DigitalOutput();
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