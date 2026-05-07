#pragma once

#include "CrossPlatformTypes.h"
#include "../base/ConfigBase.h"
#include "../base/TypedJson.h"
#include "BandSelectorConfig.h"
#include "FunCubeConfig.h"
#include "DigitalInputsConfig.h"
#include "DigitalOutputConfig.h"
#include "DigitalOutputsConfig.h"

// #ifdef USE_ETL
// #include <etl/vector.h>
// #include <etl/variant.h>
// #include <etl/optional.h>
// #else
// #include <vector>
// #include <variant>
// #include <optional>
// #endif

#ifdef USE_ETL
using etl::variant;
using etl::optional;
#else
using std::variant;
using std::optional;
#endif

namespace Config::Control
{
  static constexpr auto type = "control";
#ifdef USE_GPIO
  using SinkConfigVariant = variant<
    DigitalOutputs::Fields,
    FunCube::Fields,
    GpioLines::Fields
  >;
  using SourceConfigVariant = variant<DigitalInputs::Fields>;
#else
  using SinkConfigVariant = variant<
    FunCube::Fields
  >;
  using SourceConfigVariant = variant<std::monostate>;
#endif


#ifdef USE_ETL
  // using SinkVariant = SinkVariantT<etl::variant>;
  // using SourceVariant = SourceVariantT<etl::variant>;
  // using OptionalSinkVariant = etl::optional<SinkVariant>;
  // using OptionalSourceVariant = etl::optional<SourceVariant>;
  using SinkVector = etl::vector<SinkConfigVariant, MAX_CONTROL_SINKS>;
  using SourceVector = etl::vector<SourceConfigVariant, MAX_CONTROL_SOURCES>;
#else
  // using SinkVariant = SinkVariantT<std::variant>;
  // using SourceVariant = SourceVariantT<std::variant>;
  // using OptionalSinkVariant = std::optional<SinkVariant>;
  // using OptionalSourceVariant = std::optional<SourceVariant>;
  using SinkVector = std::vector<SinkConfigVariant>;
  using SourceVector = std::vector<SourceConfigVariant>;
#endif

  struct Fields
  {
    SinkVector sinks;
    SourceVector sources;
  };

  // extern ResultCode SinkFactory(const TypedJson& json, OptionalSinkVariant& optionalSink);
  // extern ResultCode SourceFactory(const TypedJson& json, OptionalSourceVariant& optionalSource);
  extern ResultCode fromJson(const JsonVariantConst& json, Fields& fields);
}

// using ControlConfig = Config::Control::Fields;
