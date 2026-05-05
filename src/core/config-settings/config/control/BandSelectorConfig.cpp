#include "BandSelectorConfig.h"

namespace Config::BandSelector
{
  ResultCode fromJson(JsonVariantConst json, Fields& fields)
  {
    ResultCode result = DigitalOutput::fromJson(json, fields);
    fields.type = type; // Override what was set above
    if (result != ResultCode::OK) return result;

    if (!json["defaultOut"]) return ResultCode::ERR_CONFIG_BAND_SELECTOR_MISSING_DEFAULT_OUT;
    if (!json["bands"]) return ResultCode::ERR_CONFIG_BAND_SELECTOR_MISSING_BANDS;
    auto bandsJson = json["bands"].as<JsonArrayConst>();
    if (bandsJson.size() == 0) return ResultCode::ERR_CONFIG_BAND_SELECTOR_MISSING_BANDS;

    fields.defaultOut = json["defaultOut"].as<uint32_t>();

    for (JsonVariantConst band : bandsJson) {
      Band::Fields bandFields{};
      Band::fromJson(band, bandFields);
      fields.bands.emplace_back(bandFields);
    }
    return ResultCode::OK;
  }
}