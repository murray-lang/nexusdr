//
// Created by murray on 20/8/25.
//

#include <settings/control/ControlSourceFactory.h>
#include <util/StringUtils.h>
#include "device/gpio/digital/DigitalInputGroup.h"
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
  bool gpioPresent = Gpio::isPresent();
  if(typeAslower == "digitalinputgroup" && gpioPresent)
  {
    return new DigitalInputGroup();
  }

  if(typeAslower == "gpiorotaryencoder" && gpioPresent)
  {
    return new GpioRotaryEncoder();
  }
  return nullptr;
}
