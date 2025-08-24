//
// Created by murray on 20/8/25.
//

#ifndef RADIOCONTROLCONFIG_H
#define RADIOCONTROLCONFIG_H
#include "../../io/control/config/ControlBaseConfig.h"
#include "../../io/control/config/ControlSourceConfig.h"
#include "../../config/JsonConfig.h"


class RadioControlConfig : JsonConfig
{
  // friend class RadioConfig;
public:
  RadioControlConfig() = default;
  RadioControlConfig(const RadioControlConfig& rhs) : m_sinks(rhs.m_sinks), m_sources(rhs.m_sources)
  {
  }
  ~RadioControlConfig() = default;

  RadioControlConfig& operator=(const RadioControlConfig& rhs)
  {
    m_sinks = rhs.m_sinks;
    m_sources = rhs.m_sources;
    return *this;
  }

  static RadioControlConfig fromJson(const nlohmann::json& json)
  {
    RadioControlConfig result;
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
  [[nodiscard]] const std::vector<ControlBaseConfig>& getSinks() const { return m_sinks; }
  [[nodiscard]] const std::vector<ControlBaseConfig>& getSources() const { return m_sources; }

protected:
  std::vector<ControlBaseConfig> m_sinks;
  std::vector<ControlBaseConfig> m_sources;
};

#endif //RADIOCONTROLCONFIG_H