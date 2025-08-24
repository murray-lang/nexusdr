//
// Created by murray on 2025-08-24.
//

#ifndef CUTESDR_VK6HL_CONTROLSOURCECONFIG_H
#define CUTESDR_VK6HL_CONTROLSOURCECONFIG_H
#include "../../io/control/config/ControlBaseConfig.h"
#include "../../config/JsonConfig.h"

class ControlSourceConfig : JsonConfig
{
  // friend class RadioConfig;
public:
  ControlSourceConfig() = default;
  ControlSourceConfig(const ControlSourceConfig& rhs) : m_sinks(rhs.m_sinks), m_sources(rhs.m_sources)
  {
  }
  ~ControlSourceConfig() = default;

  ControlSourceConfig& operator=(const ControlSourceConfig& rhs)
  {
    m_sinks = rhs.m_sinks;
    m_sources = rhs.m_sources;
    return *this;
  }

  static ControlSourceConfig fromJson(const nlohmann::json& json)
  {
    ControlSourceConfig result;
    if (json.contains("sinks")) {
      for (auto& controller : json["sinks"]) {
        result.m_sinks.push_back(ControlBaseConfig::fromJson(controller));
      }
    }
    if (json.contains("sources")) {
      for (auto& controller : json["sources"]) {
        result.m_sources.push_back(ControlBaseConfig::fromJson(controller));
      }
    }
    return result;
  }

  // std::vector<ControllerConfig>& getSinks() { return m_sinks; }
  const std::vector<ControlBaseConfig>& getSinks() const { return m_sinks; }
  const std::vector<ControlBaseConfig>& getSources() const { return m_sources; }

protected:
  std::vector<ControlBaseConfig> m_sinks;
  std::vector<ControlBaseConfig> m_sources;
};
#endif //CUTESDR_VK6HL_CONTROLSOURCECONFIG_H