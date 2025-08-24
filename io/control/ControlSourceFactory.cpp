//
// Created by murray on 20/8/25.
//

#include "ControlSourceFactory.h"
#include "../../util/StringUtils.h"
#include "../gpio/digital/DigitalInputGroup.h"
#include "device/platform/rpi/GpioRotaryEncoder.h"

ControlSource*
ControlSourceFactory::create(const ControlBaseConfig& config)
{
  ControlSource* result = create(config.getType());
  if (result)
  {
    result->setId(config.getType());
    result->initialise(config.getConfig());
  }
  return result;
}

ControlSource*
ControlSourceFactory::create(const std::string& type)
{
  std::string typeAslower = StringUtils::toLowerCase(type);
  if(typeAslower == "digitalInputGroup")
  {
    return new DigitalInputGroup();
  }

  if(typeAslower == "gpioRotaryEncoder")
  {
    return new GpioRotaryEncoder();
  }
  return nullptr;
}
