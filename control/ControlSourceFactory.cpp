//
// Created by murray on 20/8/25.
//

#include "ControlSourceFactory.h"

#include "config/DigitalInputGroupConfig.h"
#include "util/StringUtils.h"
#include "device/gpio/digital/DigitalInputGroup.h"
#include "device/gpio/digital/GpioRotaryEncoder.h"

ControlSource*
ControlSourceFactory::create(const ConfigBase* pConfig)
{
  ControlSource* result = create(pConfig->getType());
  if (result)
  {
    result->setId(pConfig->getType());
    result->configure(pConfig);
  }
  return result;
}

ControlSource*
ControlSourceFactory::create(const std::string& type)
{

  bool gpioPresent = Gpio::isPresent();
  if(type == DigitalInputGroupConfig::type && gpioPresent)
  {
    return new DigitalInputGroup();
  }

  // if(typeAslower == "gpiorotaryencoder" && gpioPresent)
  // {
  //   return new GpioRotaryEncoder();
  // }
  return nullptr;
}
