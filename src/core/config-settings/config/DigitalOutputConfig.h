//
// Created by murray on 25/11/25.
//

#pragma once

#include "ConfigBase.h"
#include "ConfigException.h"
#include "GpioLinesConfig.h"

class DigitalOutputConfig : public GpioLinesConfig
{
public:
  static constexpr auto type = "digitaloutput";

  DigitalOutputConfig() : GpioLinesConfig(type) {}
  DigitalOutputConfig(const char* typeName) : GpioLinesConfig(typeName) {}
  ~DigitalOutputConfig() override  = default;

  void fromJson(JsonVariantConst json) override
  {
    GpioLinesConfig::fromJson(json);
    if (lines.empty()) {
      throw ConfigException("DigitalOutputConfig: lines empty");
    }
    if (json["settingPath"]) {
      m_settingPath = json["settingPath"].as<const char *>();
    } else {
      throw ConfigException("DigitalOutputConfig: settingPath empty");
    }
  }
  [[nodiscard]] const std::string& getSettingPath() const { return m_settingPath; }

protected:
  std::string m_settingPath;
};