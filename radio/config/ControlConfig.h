//
// Created by murray on 5/08/25.
//

#ifndef CONTROLCONFIG_H
#define CONTROLCONFIG_H
#include "./JsonConfig.h"

class ControlConfig : public JsonConfig
{
  friend class TransmitterConfig;
  friend class ReceiverConfig;
public:
  explicit ControlConfig() = default;

  ControlConfig(const ControlConfig& rhs) :
    m_type(rhs.m_type),
    m_config(rhs.m_config)
  {
  }
  ControlConfig& operator=(const ControlConfig& rhs)
  {
    m_type = rhs.m_type;
    m_config = rhs.m_config;
    return *this;
  }

  static ControlConfig fromJson(const nlohmann::json& json)
  {
    ControlConfig result;
    if (json.contains("type")) {
      result.m_type = json["type"].get<std::string>();
    }
    if (json.contains("config")) {
      result.m_config = json["config"];
    }
    return result;
  }

  [[nodiscard]] const std::string& getType() const { return m_type; }
  [[nodiscard]] const nlohmann::json& getConfig() const { return m_config; }

protected:
  std::string m_type;
  nlohmann::json m_config;
};

#endif //CONTROLCONFIG_H
