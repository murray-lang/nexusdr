#pragma once

#include "core/config-settings/config/control/DigitalInputConfig.h"
#include "DigitalInput.h"
#include "GpioRotaryEncoder.h"
#include "core/config-settings/config/control/DigitalInputsConfig.h"

#ifdef USE_ETL_COLLECTIONS
#include "etl/variant"

using DigitalInputVariant = etl::variant<DigitalInput, GpioRotaryEncoder>;
#else
#include <variant>

using DigitalInputVariant = std::variant<DigitalInput, GpioRotaryEncoder>;
#endif

class DigitalInputFactory
{
public:
  static ResultCode create(const Config::DigitalInputs::DigitalInputVariant& config, DigitalInputVariant& input);
};
