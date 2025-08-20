//
// Created by murray on 20/8/25.
//

#ifndef CONTROLLERSCONFIG_H
#define CONTROLLERSCONFIG_H
#include "ControllerConfig.h"
#include "../../../config/JsonConfig.h"

class ControllersConfig : JsonConfig
{
  // friend class RadioConfig;
public:
  ControllersConfig() = default;
  ControllersConfig(const ControllersConfig& rhs) : m_sinks(rhs.m_sinks), m_sources(rhs.m_sources)
  {
  }
  ~ControllersConfig() = default;

  ControllersConfig& operator=(const ControllersConfig& rhs)
  {
    m_sinks = rhs.m_sinks;
    m_sources = rhs.m_sources;
    return *this;
  }

  static ControllersConfig fromJson(const nlohmann::json& json)
  {
    ControllersConfig result;
    if (json.contains("sinks")) {
      for (auto& controller : json["sinks"]) {
        result.m_sinks.push_back(ControllerConfig::fromJson(controller));
      }
    }
    if (json.contains("sources")) {
      for (auto& controller : json["sources"]) {
        result.m_sources.push_back(ControllerConfig::fromJson(controller));
      }
    }
    return result;
  }

  // std::vector<ControllerConfig>& getSinks() { return m_sinks; }
  const std::vector<ControllerConfig>& getSinks() const { return m_sinks; }
  const std::vector<ControllerConfig>& getSources() const { return m_sources; }

protected:
  std::vector<ControllerConfig> m_sinks;
  std::vector<ControllerConfig> m_sources;
};

#endif //CONTROLLERSCONFIG_H