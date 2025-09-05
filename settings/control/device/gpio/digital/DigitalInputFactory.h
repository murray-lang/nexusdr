//
// Created by murray on 2025-09-04.
//

#ifndef CUTESDR_VK6HL_DIGITALINPUTFACTORY_H
#define CUTESDR_VK6HL_DIGITALINPUTFACTORY_H
#include "DigitalInput.h"
#include "config/ControlBaseConfig.h"

class DigitalInputFactory
{
public:
  static DigitalInput* create(const ControlBaseConfig& config, GpioLines& lines);
  static DigitalInput* create(const std::string& type, GpioLines& lines);
};


#endif //CUTESDR_VK6HL_DIGITALINPUTFACTORY_H