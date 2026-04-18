//
// Created by murray on 15/9/25.
//

#pragma once

#include "../base/ConfigBase.h"
#include <etl/optional.h>
#include <etl/variant.h>

#include "DigitalInputConfig.h"
#include "RotaryEncoderConfig.h"
#include "../base/TypedJson.h"

#ifdef USE_ETL_COLLECTIONS
#include <etl/vector.h>
#include <etl/variant.h>
#include <etl/optional.h>
#else
#include <vector>
#include <variant>
#include <optional>
#endif

namespace Config::DigitalInputs
{
  static constexpr auto type = "digitalinputs";

  template<template<typename...> class Variant>
  using DigitalInputVariantT = Variant<
    DigitalInput::Fields,
    RotaryEncoder::Fields
  >;

#ifdef USE_ETL_COLLECTIONS
  using DigitalInputVariant = DigitalInputVariantT<etl::variant>;
  using OptionalVariant = etl::optional<DigitalInputVariant>;
  using DigitalInputVector  = etl::vector<OptionalVariant, MAX_DIGITAL_INPUTS>;
#else
  using DigitalInputVariant = DigitalInputVariantT<std::variant>;
  using OptionalVariant = std::optional<DigitalInputVariant>;
  using DigitalInputVector  = std::vector<OptionalVariant>;
#endif

  struct Fields : Alternative
  {
    DigitalInputVector inputs;
  };

  static Result factory(const TypedJson& json, OptionalVariant& optionalVariant)
  {
    Result result = Result::OK;
    if (json.type == DigitalInput::type) {
      DigitalInput::Fields fields{};
      result = DigitalInput::fromJson(json.config, fields);

      if (result == Result::OK) {
        optionalVariant.emplace(fields);
      }
      return result;
    }
    if (json.type == RotaryEncoder::type) {
      RotaryEncoder::Fields fields{};
      result = RotaryEncoder::fromJson(json.config, fields);

      if (result == Result::OK) {
        optionalVariant.emplace(fields);
      }
      return result;
    }
    return Result::UNKNOWN_TYPE;
  }

  static Result fromJson(const TypedJson& json, Fields& fields)
  {
    fields.type = type;
    fields.inputs.clear();

    Result result = Result::OK;
    if (json.config["inputs"]) {
      for (JsonVariantConst inputJson : json.config["inputs"].as<JsonArrayConst>()) {
        TypedJson typedJson;
        result = typedJson.fromJson(inputJson);

        if (result != Result::OK) return result;

        OptionalVariant inputOpt;
        result = factory(typedJson, inputOpt);

        if (result != Result::OK) return result;

        fields.inputs.emplace_back(inputOpt);
      }
    }
    return Result::OK;
  }
}
