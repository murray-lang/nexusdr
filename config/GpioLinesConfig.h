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

  void fromJson(const nlohmann::json& json) override
  {
    if (json.contains("lines")) {
      for (auto& line : json["lines"]) {
        lines.push_back(line);
      }
    }
    if (json.contains("direction")) {
      direction = json["direction"];
    }
    if (json.contains("bias")) {
      bias = json["bias"];
    }
    if (json.contains("edge")) {
      edge = json["edge"];
    }
  }
};
