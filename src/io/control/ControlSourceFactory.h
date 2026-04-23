#pragma once
#include "core/config-settings/config/control/ControlConfig.h"

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

#include "ControlSource.h"
// #include "core/config-settings/config/base/ConfigBase.h"

#ifdef USE_GPIO
#include "device/gpio/digital/DigitalInputs.h"

using ControlSourceVariant = variant< DigitalInputs>;
#else
using ControlSourceVariant = variant<std::monostate>;
#endif


class ControlSourceFactory
{
public:
  static ResultCode create(const Config::Control::SourceVariant& config, ControlSourceVariant& source);
};

