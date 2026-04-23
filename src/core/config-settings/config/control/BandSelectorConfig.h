//
// Created by murray on 16/12/25.
//

#pragma once

#include "DigitalOutputConfig.h"
#include "BandSelectorBandsConfig.h"

#ifdef USE_ETL_COLLECTIONS
#include <etl/vector.h>
#include <etl/optional.h>
#else
#include <vector>
#include <optional>
#endif


#define MAX_BAND_SELECTOR_BANDS 13

namespace Config::DigitalOutput::BandSelector
{
  static constexpr auto type = "bandselector";

#ifdef USE_ETL_COLLECTIONS
  using OptionalBandConfig = etl::optional<Band::Fields>;
  using BandsVector = etl::vector<OptionalBandConfig, MAX_BAND_SELECTOR_BANDS>;
#else
  using OptionalBandConfig = std::optional<Band::Fields>;
  using BandsVector = std::vector<Band::Fields>;
#endif

  struct Fields : DigitalOutput::Fields
  {
    uint32_t defaultOut{};
    BandsVector bands;
  };

  extern Result fromJson(JsonVariantConst json, Fields& fields);
}

// using BandSelectorConfig = Config::DigitalOutput::BandSelector::Fields;
