//
// Created by murray on 29/9/25.
//

#pragma once

#include "ConfigBase.h"

struct GpioLinesConfigFields
{
  std::vector<uint32_t> lines;
  std::string direction; // "input" or "output"
  std::string bias; //"none", "pull-up" or "pull-down"
  std::string edge; //"rising", "falling" or "both"
};

class GpioLinesConfig : public ConfigBase, public GpioLinesConfigFields
{
public:
  explicit GpioLinesConfig(const char* type) : ConfigBase(type) {}
  ~GpioLinesConfig() override = default;
  GpioLinesConfig(const GpioLinesConfig& rhs) = default;

  GpioLinesConfig& operator=(const GpioLinesConfig& rhs)
  {
    if (this != &rhs) {
      static_cast<GpioLinesConfigFields&>(*this) = static_cast<const GpioLinesConfigFields&>(rhs);
    }
    return *this;
  }

  void setFields(const GpioLinesConfigFields& f)
  {
    static_cast<GpioLinesConfigFields&>(*this) = f;
  }

  [[nodiscard]] GpioLinesConfigFields getFields() const
  {
    return static_cast<const GpioLinesConfigFields&>(*this);
  }

  void fromJson(JsonVariantConst json) override
  {
    if (json["lines"]) {
      for (JsonVariantConst line : json["lines"].as<JsonArrayConst>()) {
        lines.push_back(line.as<uint32_t>());
      }
    }
    if (json["direction"]) {
      direction = json["direction"].as<const char *>();
    }
    if (json["bias"]) {
      bias = json["bias"].as<const char *>();
    }
    if (json["edge"]) {
      edge = json["edge"].as<const char *>();
    }
  }
};
