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

  static Result fromJson(JsonVariantConst json, Fields& fields)
  {
    Result result = DigitalOutput::fromJson(json, fields);
    fields.type = type; // Override what was set above
    if (result != Result::OK) return result;

    if (!json["defaultOut"]) return Result::BAND_SELECTOR_MISSING_DEFAULT_OUT;
    if (!json["bands"]) return Result::BAND_SELECTOR_MISSING_BANDS;
    auto bandsJson = json["bands"].as<JsonArrayConst>();
    if (bandsJson.size() == 0) return Result::BAND_SELECTOR_MISSING_BANDS;

    fields.defaultOut = json["defaultOut"].as<uint32_t>();

    for (JsonVariantConst band : bandsJson) {
      Band::Fields bandFields{};
      Band::fromJson(band, bandFields);
      fields.bands.emplace_back(bandFields);
    }
    return Result::OK;
  }
}
