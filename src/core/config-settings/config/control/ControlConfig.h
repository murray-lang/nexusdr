#pragma once

#include "../base/ConfigBase.h"
#include "../base/TypedJson.h"


#include "BandSelectorConfig.h"
#include "FunCubeConfig.h"
#include "DigitalInputsConfig.h"
#include "DigitalOutputConfig.h"

// #define USE_ETL_COLLECTIONS

#ifdef USE_ETL_COLLECTIONS
#include <etl/vector.h>
#include <etl/variant.h>
#include <etl/optional.h>
#include <etl/string.h>
#else
#include <vector>
#include <variant>
#include <optional>
#include <string>
#endif

#define MAX_CONTROL_SINKS 4
#define MAX_CONTROL_SOURCES 4

namespace Config::Control
{
  static constexpr auto type = "control";

  template<template<typename...> class Variant>
  using SinkVariantT = Variant<
    DigitalOutput::Fields,
    FunCube::Fields,
    DigitalOutput::BandSelector::Fields
  >;

  template<template<typename...> class Variant>
  using SourceVariantT = Variant<
    DigitalInputs::Fields
  >;

#ifdef USE_ETL_COLLECTIONS
  using SinkVariant = SinkVariantT<etl::variant>;
  using SourceVariant = SourceVariantT<etl::variant>;
  using OptionalSinkVariant = etl::optional<SinkVariant>;
  using OptionalSourceVariant = etl::optional<SourceVariant>;
  using SinkVector = etl::vector<OptionalSinkVariant, MAX_CONTROL_SINKS>;
  using SourceVector = etl::vector<OptionalSourceVariant, MAX_CONTROL_SOURCES>;
#else
  using SinkVariant = SinkVariantT<std::variant>;
  using SourceVariant = SourceVariantT<std::variant>;
  using OptionalSinkVariant = std::optional<SinkVariant>;
  using OptionalSourceVariant = std::optional<SourceVariant>;
  using SinkVector = std::vector<OptionalSinkVariant>;
  using SourceVector = std::vector<OptionalSourceVariant>;
#endif

  struct Fields
  {
    SinkVector sinks;
    SourceVector sources;
  };

  static Result SinkFactory(const TypedJson& json, OptionalSinkVariant& optionalSink)
  {
    Result result = Result::OK;
    if (json.type == DigitalOutput::type) {
      DigitalOutput::Fields fields{};
      result = DigitalOutput::fromJson(json.config, fields);
      if (result == Result::OK) {
        optionalSink.emplace(fields);
      }
      return result;
    }
    if (json.type == FunCube::type) {
      FunCube::Fields fields{};
      result = FunCube::fromJson(json.config, fields);
      if (result == Result::OK) {
        optionalSink.emplace(fields);
      }
      return result;
    }
    if (json.type == DigitalOutput::BandSelector::type) {
      DigitalOutput::BandSelector::Fields fields{};
      result = DigitalOutput::BandSelector::fromJson(json.config, fields);
      if (result == Result::OK) {
        optionalSink.emplace(fields);
      }
      return result;
    }
    return Result::UNKNOWN_TYPE;
  }

  static Result SourceFactory(const TypedJson& json, OptionalSourceVariant& optionalSource)
  {
    Result result = Result::OK;
    if (json.type == DigitalInputs::type) {
      DigitalInputs::Fields fields{};
      result = DigitalInputs::fromJson(json, fields);
      if (result == Result::OK) {
        optionalSource.emplace(fields);
      }
      return result;
    }
    return Result::UNKNOWN_TYPE;
  }

  static Result fromJson(const TypedJson& json, Fields& fields)
  {
    fields.sinks.clear();
    fields.sources.clear();

    Result result = Result::OK;
    if (json.config["sinks"]) {
      for (JsonVariantConst sinkJson : json.config["sinks"].as<JsonArrayConst>()) {
        TypedJson taggedJson;
        result = taggedJson.fromJson(sinkJson);

        if (result != Result::OK) return result;

        OptionalSinkVariant sinkOpt;
        result = SinkFactory(taggedJson, sinkOpt);

        if (result != Result::OK) return result;

        fields.sinks.emplace_back(sinkOpt);
      }
    }
    if (json.config["sources"]) {
      for (JsonVariantConst sourceJson : json.config["sources"].as<JsonArrayConst>()) {
        TypedJson taggedJson;
        result = taggedJson.fromJson(sourceJson);

        if (result != Result::OK) return result;

        OptionalSourceVariant sourceOpt;
        result = SourceFactory(taggedJson, sourceOpt);

        if (result != Result::OK) return result;

        fields.sources.emplace_back(sourceOpt);
      }
    }
    return Result::OK;
  }
}
