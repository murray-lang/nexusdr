//
// Created by murray on 2025-09-04.
//

#include "DigitalInputFactory.h"

#include "DigitalInputs.h"
#include "core/util/StringUtils.h"

DigitalInput*
DigitalInputFactory::create(const DigitalInputConfig* pConfig)
{
  DigitalInput* result = create(pConfig->getType());
  if (result)  {
    result->configure(pConfig);
  }
  return result;
}

DigitalInput*
DigitalInputFactory::create(const std::string& type)
{
  std::string typeAslower = StringUtils::toLowerCase(type);
  bool gpioPresent = Gpio::isPresent();
  if(typeAslower == "rotaryencoder" && gpioPresent)
  {
    return new GpioRotaryEncoder();
  }
  if(typeAslower == "digitalinput" && gpioPresent)
  {
    return new DigitalInput();
  }

  // if(typeAslower == "gpiorotaryencoder" && gpioPresent)
  // {
  //   return new GpioRotaryEncoder();
  // }
  return nullptr;
}