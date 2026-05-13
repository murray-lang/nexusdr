#pragma once
#include "core/config-settings/config/control/ControlConfig.h"

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

#include "ControlSource.h"
#ifdef USE_GPIO
#include "device/gpio/digital/DigitalInputs.h"
#endif

#ifdef IS_QT
#include "qt/QtControlSource.h"
#endif
// #include "core/config-settings/config/base/ConfigBase.h"

#ifdef USE_GPIO
  #ifdef IS_QT
    using ControlSourceVariant = variant<QtControlSource, DigitalInputs>;
  #else
    using ControlSourceVariant = variant<DigitalInputs>;
  #endif // IS_QT
#else
  #ifdef IS_QT
    using ControlSourceVariant = variant<QtControlSource>;
  #else
    using ControlSourceVariant = variant<std::monostate>;
  #endif
#endif


class ControlSourceFactory
{
public:
  static ResultCode create(const Config::Control::SourceConfigVariant& config, ControlSourceVariant& source);
};

