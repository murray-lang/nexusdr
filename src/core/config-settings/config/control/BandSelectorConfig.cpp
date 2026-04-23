#include "BandSelectorConfig.h"

namespace Config::DigitalOutput::BandSelector
{
  Result fromJson(JsonVariantConst json, Fields& fields)
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