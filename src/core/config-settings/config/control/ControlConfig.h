#pragma once

#include "../base/ConfigBase.h"
#include "../base/TypedJson.h"
#include "BandSelectorConfig.h"
#include "FunCubeConfig.h"
#include "DigitalInputsConfig.h"
#include "DigitalOutputConfig.h"

#ifdef USE_ETL_COLLECTIONS
#include <etl/vector.h>
#include <etl/variant.h>
#include <etl/optional.h>
#else
#include <vector>
#include <variant>
#include <optional>
#endif

#ifdef USE_ETL_COLLECTIONS
using etl::variant;
using etl::optional;
#else
using std::variant;
using std::optional;
#endif

namespace Config::Control
{
  static constexpr auto type = "control";

  using SinkVariant = variant<
    DigitalOutput::Fields,
    FunCube::Fields,
    DigitalOutput::BandSelector::Fields,
    GpioLines::Fields
  >;
  using SourceVariant = variant<DigitalInputs::Fields>;

  // using OptionalSinkVariant = optional<SinkVariant>;
  // using OptionalSourceVariant = optional<SourceVariant>;

  // template<template<typename...> class Variant>
  // using SinkVariantT = Variant<
  //   DigitalOutput::Fields,
  //   FunCube::Fields,
  //   DigitalOutput::BandSelector::Fields
  // >;
  //
  // template<template<typename...> class Variant>
  // using SourceVariantT = Variant<
  //   DigitalInputs::Fields
  // >;

#ifdef USE_ETL_COLLECTIONS
  // using SinkVariant = SinkVariantT<etl::variant>;
  // using SourceVariant = SourceVariantT<etl::variant>;
  // using OptionalSinkVariant = etl::optional<SinkVariant>;
  // using OptionalSourceVariant = etl::optional<SourceVariant>;
  using SinkVector = etl::vector<OptionalSinkVariant, MAX_CONTROL_SINKS>;
  using SourceVector = etl::vector<OptionalSourceVariant, MAX_CONTROL_SOURCES>;
#else
  // using SinkVariant = SinkVariantT<std::variant>;
  // using SourceVariant = SourceVariantT<std::variant>;
  // using OptionalSinkVariant = std::optional<SinkVariant>;
  // using OptionalSourceVariant = std::optional<SourceVariant>;
  using SinkVector = std::vector<SinkVariant>;
  using SourceVector = std::vector<SourceVariant>;
#endif

  struct Fields
  {
    SinkVector sinks;
    SourceVector sources;
  };

  // extern Result SinkFactory(const TypedJson& json, OptionalSinkVariant& optionalSink);
  // extern Result SourceFactory(const TypedJson& json, OptionalSourceVariant& optionalSource);
  extern Result fromJson(const TypedJson& json, Fields& fields);
}

// using ControlConfig = Config::Control::Fields;
