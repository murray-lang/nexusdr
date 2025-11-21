//
// Created by murray on 2025-09-04.
//

#pragma once

#include <config/DigitalInputConfig.h>
#include "DigitalInput.h"

class DigitalInputFactory
{
public:
  static DigitalInput* create(const DigitalInputConfig* pConfig);
  static DigitalInput* create(const std::string& type);
};
