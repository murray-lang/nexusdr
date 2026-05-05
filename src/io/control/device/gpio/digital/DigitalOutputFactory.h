#pragma once

#include "DigitalOutputTypes.h"
#include "core/config-settings/config/control/DigitalOutputsConfig.h"

class DigitalOutputFactory
{
public:
  static ResultCode create(const Config::DigitalOutputs::DigitalOutputConfigVariant& config, DigitalOutputVariant& output);
};
