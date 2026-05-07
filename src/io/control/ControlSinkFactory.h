//
// Created by murray on 6/08/25.
//

#pragma once
#include "device/gpio/digital/DigitalOutputs.h"
#ifdef USE_ETL
#include <etl/vector.h>
#include <etl/variant.h>
#include <etl/optional.h>
#include <etl/string.h>
#include <etl/memory.h>
#else
#include <vector>
#include <variant>
#include <optional>
#include <string>
#include <memory>
#endif

#ifdef USE_ETL
using etl::variant;
using etl::optional;
using etl::unique_ptr;
#else
using std::variant;
using std::optional;
using std::unique_ptr;
#endif

#include <string>

#include "device/FunCubeDongle/FunCubeDongle.h"
#include "device/gpio/digital/GpioBandSelector.h"
#include "device/gpio/digital/DigitalOutput.h"
#include "core/config-settings/config/control/ControlConfig.h"
#include "ResultCode.h"

#ifdef USE_GPIO
using ControlSinkVariant = variant< FunCubeDongle, DigitalOutputs>;
#else
using ControlSinkVariant = variant<FunCubeDongle>;
#endif


class ControlSinkFactory
{
public:
  static ResultCode create(const Config::Control::SinkConfigVariant& config, ControlSinkVariant& sink);
};
