#pragma once

#include <iostream>
#include "../../Gpio.h"
#include <gpiod.h>
#include <memory>

class GpioImplGpiod: public Gpio::Impl
{
  constexpr static auto defaultChipPath = "/dev/gpiochip0";
public:
  GpioImplGpiod();
  ~GpioImplGpiod() override;
    
    static bool isPresent();
    bool open() override;
    bool close() override;

  DigitalInputsRequest* requestDigitalInputs(const char * contextId, const std::vector<DigitalInput*>& lines) override;

protected:
    gpiod_chip* m_pChip;
};
