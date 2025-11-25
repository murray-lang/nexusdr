//
// Created by murray on 25/11/25.
//

#pragma once
#include <cstdint>
#include <vector>
// #include "DigitalOutput.h"
// #include "io/control/device/gpio/Gpio.h"

class Gpio;
class DigitalOutput;

class DigitalOutputLinesRequest
{
public:
  DigitalOutputLinesRequest();
  virtual ~DigitalOutputLinesRequest() = default;

  virtual void request(const char * contextId, const std::vector<DigitalOutput*>& inputs) = 0;

  virtual void release() = 0;
  virtual int setLineValue(uint32_t line, bool value) = 0;

protected:
  Gpio& m_gpio;
};

