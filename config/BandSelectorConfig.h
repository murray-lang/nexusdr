//
// Created by murray on 16/12/25.
//

#pragma once

#include "ConfigException.h"
#include "DigitalOutputConfig.h"
#include "BandSelectorLookupItemConfig.h"


struct BandSelectorConfigFields
{
  uint32_t defaultOut{};
  std::vector<BandSelectorLookupItemConfigFields> bands;
};

class BandSelectorConfig : public DigitalOutputConfig, public BandSelectorConfigFields
{
public:
  static constexpr auto type = "bandselector";

  BandSelectorConfig() : DigitalOutputConfig(type) {}
  ~BandSelectorConfig() override  = default;

  void fromJson(const nlohmann::json& json) override
  {
    DigitalOutputConfig::fromJson(json);
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
