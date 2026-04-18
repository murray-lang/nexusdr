#pragma once

#include "../base/ConfigBase.h"

namespace Config::DigitalOutput::BandSelector::Band
{
  // static constexpr auto tag = "bandSelectorBand";

  struct Fields
  {
    uint32_t fromFrequency;
    uint32_t toFrequency;
    uint32_t outValue;
  };

  static Result fromJson(JsonVariantConst json, Fields& fields)
  {
    if (json["fromFrequency"]) {
      fields.fromFrequency = json["fromFrequency"];
    } else {
      return Result::BAND_MISSING_FROM_FREQUENCY;
    }
    if (json["toFrequency"]) {
      fields.toFrequency = json["toFrequency"];
    } else {
      return Result::BAND_MISSING_TO_FREQUENCY;
    }
    if (json["outValue"]) {
      fields.outValue = json["outValue"];
    } else {
      return Result::BAND_MISSING_OUT_VALUE;
    }
    return Result::OK;
  }
}
