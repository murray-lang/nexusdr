//
// Created by murray on 16/12/25.
//

#pragma once

#include "ConfigBase.h"
#include "ConfigException.h"

struct BandSelectorLookupItemConfigFields
{
  uint32_t fromFrequency;
  uint32_t toFrequency;
  uint32_t out;
};

class BandSelectorLookupItemConfig : public ConfigBase, public BandSelectorLookupItemConfigFields
{
public:
  static constexpr auto type = "bandSelectorLookupItem";
  BandSelectorLookupItemConfig() : ConfigBase(type), BandSelectorLookupItemConfigFields()  {}
  ~BandSelectorLookupItemConfig() override = default;

  BandSelectorLookupItemConfig(const BandSelectorLookupItemConfig& rhs) = default;
  BandSelectorLookupItemConfig& operator=(const BandSelectorLookupItemConfig& rhs) = default;

  void fromJson(JsonVariantConst json) override
  {
    if (json["fromFrequency"]) {
      fromFrequency = json["fromFrequency"];
    } else {
      throw ConfigException("BandSelectorLookupItemConfig: fromFrequency empty");
    }
    if (json["toFrequency"]) {
      toFrequency = json["toFrequency"];
    } else {
      throw ConfigException("BandSelectorLookupItemConfig: toFrequency empty");
    }
    if (json["out"]) {
      out = json["out"];
    } else {
      throw ConfigException("BandSelectorLookupItemConfig: out empty");
    }
  }
};
