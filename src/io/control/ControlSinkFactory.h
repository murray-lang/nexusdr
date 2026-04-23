//
// Created by murray on 6/08/25.
//

#pragma once
#ifdef USE_ETL_COLLECTIONS
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

#ifdef USE_ETL_COLLECTIONS
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
using ControlSinkVariant = variant< FunCubeDongle, GpioBandSelector, DigitalOutput>;
#else
using ControlSinkVariant = variant<FunCubeDongle>;
#endif


class ControlSinkFactory
{
public:
  static ResultCode create(const Config::Control::SinkVariant& config, ControlSinkVariant& sink);
};
