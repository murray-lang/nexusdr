//
// Created by murray on 6/08/25.
//
#include "ControlSinkFactory.h"
#include <nlohmann/json.hpp>

#include "core/config-settings/config/BandSelectorConfig.h"
#include "core/config-settings/config/FunCubeConfig.h"
#include "device/FunCubeDongle/FunCubeDongle.h"
#include "device/gpio/digital/GpioBandSelector.h"
#include "device/gpio/digital/DigitalOutput.h"
#include "core/util/StringUtils.h"

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
  std::string typeAsLower = StringUtils::toLowerCase(type);
  if(typeAsLower == FunCubeConfig::type) {
    return new FunCubeDongle();
  }
#ifdef USE_GPIO
  if(typeAsLower == DigitalOutputConfig::type) {
    return new DigitalOutput();
  }
  if (typeAsLower == BandSelectorConfig::type) {
    return new GpioBandSelector();
  }
#endif
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