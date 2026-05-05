#pragma once

#include "../base/ConfigBase.h"

namespace Config::BandSelector::Band
{
  // static constexpr auto tag = "bandSelectorBand";

  struct Fields
  {
    uint32_t fromFrequency;
    uint32_t toFrequency;
    uint32_t outValue;
  };

  extern ResultCode fromJson(JsonVariantConst json, Fields& fields);
}

// using BandSelectorBandsConfig = Config::BandSelector::Band::Fields;
