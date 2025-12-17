//
// Created by murray on 16/12/25.
//

#pragma once

#include "ConfigException.h"
#include "GpioLinesConfig.h"
#include "BandSelectorLookupItemConfig.h"


struct BandSelectorConfigFields
{
  uint32_t defaultOut{};
  std::vector<BandSelectorLookupItemConfigFields> bands;
};

class BandSelectorConfig : public GpioLinesConfig, public BandSelectorConfigFields
{
public:
  static constexpr auto type = "bandSelector";

  BandSelectorConfig() : GpioLinesConfig(type) {}
  ~BandSelectorConfig() override  = default;

  void fromJson(const nlohmann::json& json) override
  {
    GpioLinesConfig::fromJson(json);
    if (lines.empty()) {
      throw ConfigException("BandSelectorConfig: lines empty");
    }

    if (json.contains("defaultOut")) {
      defaultOut = json["defaultOut"];
    } else {
      throw ConfigException("BandSelectorConfig: defaultOut empty");
    }

    if (json.contains("bands")) {
      for (auto& lookup : json["bands"]) {
        BandSelectorLookupItemConfig item;
        item.fromJson(lookup);
        bands.emplace_back(item);
      }
    } else {
      throw ConfigException("BandSelectorConfig: bands empty");
    }

  }
};
