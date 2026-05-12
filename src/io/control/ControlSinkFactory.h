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
#ifdef USE_GPIO
#include "device/gpio/digital/DigitalOutputs.h"
#include "device/gpio/digital/GpioBandSelector.h"
#include "device/gpio/digital/DigitalOutput.h"
#endif

#ifdef IS_QT
#include "qt/QtControlSink.h"
#endif

#include "device/FunCubeDongle/FunCubeDongle.h"

#include "core/config-settings/config/control/ControlConfig.h"
#include "ResultCode.h"

#ifdef USE_GPIO
  #ifdef IS_QT
    using ControlSinkVariant = variant<FunCubeDongle, DigitalOutputs, QtControlSink>;
  #else
    using ControlSinkVariant = variant<FunCubeDongle, DigitalOutputs>;
  #endif
#else
#ifdef IS_QT
  using ControlSinkVariant = variant<FunCubeDongle, QtControlSink>;
#else
  using ControlSinkVariant = variant<FunCubeDongle>;
#endif
#endif


class ControlSinkFactory
{
public:
  static ResultCode create(const Config::Control::SinkConfigVariant& config, ControlSinkVariant& sink);
};
