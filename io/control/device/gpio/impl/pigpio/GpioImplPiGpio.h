#pragma once

#include <iostream>
#include "../../Gpio.h"
#include <pigpio.h>
#include <memory>

class GpioImplPiGpio: public Gpio::Impl
{
public:
  GpioImplPiGpio();
  ~GpioImplPiGpio() override;
    
    static bool isPresent();
    bool open() override;
    bool close() override;

  DigitalInputLinesRequest* requestDigitalInputs(const char * contextId, const std::vector<DigitalInput*>& lines) override;
  DigitalOutputLinesRequest* requestDigitalOutputs(const char * contextId, const std::vector<DigitalOutput*>& lines) override;

protected:
    int m_initRc;
};
