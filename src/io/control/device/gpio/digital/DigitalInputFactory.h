#pragma once

#include "DigitalInputTypes.h"
#include "core/config-settings/config/control/DigitalInputsConfig.h"

class DigitalInputFactory
{
public:
  static ResultCode create(const Config::DigitalInputs::DigitalInputConfigVariant& config, DigitalInputVariant& input);
};
