#include "BandSelectorBandsConfig.h"

namespace Config::BandSelector::Band
{
  ResultCode fromJson(JsonVariantConst json, Fields& fields)
  {
    if (json["fromFrequency"].is<JsonVariantConst>()) {
      fields.fromFrequency = json["fromFrequency"];
    } else {
      return ResultCode::ERR_CONFIG_BAND_MISSING_FROM_FREQUENCY;
    }
    if (json["toFrequency"].is<JsonVariantConst>()) {
      fields.toFrequency = json["toFrequency"];
    } else {
      return ResultCode::ERR_CONFIG_BAND_MISSING_TO_FREQUENCY;
    }
    if (json["outValue"].is<JsonVariantConst>()) {
      fields.outValue = json["outValue"];
    } else {
      return ResultCode::ERR_CONFIG_BAND_MISSING_OUT_VALUE;
    }
    return ResultCode::OK;
  }
}
