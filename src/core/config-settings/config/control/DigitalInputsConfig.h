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

using etl::variant;
#else
#include <vector>
#include <variant>
#include <optional>

using std::variant;
#endif

namespace Config::DigitalInputs
{
  static constexpr auto type = "digitalinputs";

  using DigitalInputVariant = variant<DigitalInput::Fields, RotaryEncoder::Fields>;

#ifdef USE_ETL_COLLECTIONS
  using DigitalInputVector  = etl::vector<DigitalInputVariant, MAX_DIGITAL_INPUTS>;
#else
  using DigitalInputVector  = std::vector<DigitalInputVariant>;
#endif

  struct Fields : Alternative
  {
    DigitalInputVector inputs;
  };

  extern Result fromJson(const TypedJson& json, Fields& fields);
}

// using DigitalInputsConfig = Config::DigitalInputs::Fields;
