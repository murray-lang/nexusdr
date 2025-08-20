//
// Created by murray on 20/8/25.
//

#include "SourceControllerFactory.h"

#include "device/platform/rpi/GpioRotaryEncoder.h"

SourceController*
SourceControllerFactory::create(const ControllerConfig& config)
{
  SourceController* result = create(config.getType());
  if (result)
  {
    result->setId(config.getType());
    result->initialise(config.getConfig());
  }
  return result;
}

SourceController*
SourceControllerFactory::create(const std::string& type)
{
  std::string typeAslower = toLowerCase(type);
  if(typeAslower == "gpiorotaryencoder")
  {
    return new GpioRotaryEncoder();
  }
  return nullptr;
}

std::string
SourceControllerFactory::toLowerCase(const std::string& str)
{
  std::string output = str;
  std::transform(output.begin(), output.end(), output.begin(),
                 [](unsigned char c){ return std::tolower(c); });
  return output;

}