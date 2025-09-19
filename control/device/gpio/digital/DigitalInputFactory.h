//
// Created by murray on 2025-09-04.
//

#ifndef CUTESDR_VK6HL_DIGITALINPUTFACTORY_H
#define CUTESDR_VK6HL_DIGITALINPUTFACTORY_H
#include <config/DigitalInputConfig.h>
#include "DigitalInput.h"

class DigitalInputFactory
{
public:
  static DigitalInput* create(const DigitalInputConfig* pConfig);
  static DigitalInput* create(const std::string& type);
};


#endif //CUTESDR_VK6HL_DIGITALINPUTFACTORY_H