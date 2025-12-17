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
  ~DigitalOutputConfig() override  = default;

  void fromJson(const nlohmann::json& json) override
  {
    GpioLinesConfig::fromJson(json);
    if (lines.empty()) {
      throw ConfigException("DigitalOutputConfig: lines empty");
    }
    if (json.contains("settingPath")) {
      m_settingPath = json["settingPath"];
    } else {
      throw ConfigException("DigitalOutputConfig: settingPath empty");
    }
  }
  [[nodiscard]] const std::string& getSettingPath() const { return m_settingPath; }

protected:
  std::string m_settingPath;
};