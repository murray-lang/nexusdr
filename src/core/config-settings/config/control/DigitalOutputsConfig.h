//
// Created by murray on 15/9/25.
//

#pragma once

#include "../base/ConfigBase.h"
#include "CrossPlatformTypes.h"

#include "DigitalOutputConfig.h"
#include "BandSelectorConfig.h"
#include "../base/TypedJson.h"

#ifdef USE_ETL
#include <etl/vector.h>
#else
#include <vector>
#endif

namespace Config::DigitalOutputs
{
  static constexpr auto type = "digitaloutputs";

  using DigitalOutputConfigVariant = variant<DigitalOutput::Fields, BandSelector::Fields>;

#ifdef USE_ETL
  using DigitalOutputConfigVector  = etl::vector<DigitalOutputConfigVariant, MAX_DIGITAL_OUTPUT_HANDLERS>;
#else
  using DigitalOutputConfigVector  = std::vector<DigitalOutputConfigVariant>;
#endif

  struct Fields : Alternative
  {
    DigitalOutputConfigVector outputs;
  };

  extern ResultCode fromJson(const JsonDocument& json, Fields& fields);
}
