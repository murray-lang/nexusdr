//
// Created by murray on 5/08/25.
//

#ifndef CONTROLBASECONFIG_H
#define CONTROLBASECONFIG_H
#include "../../../config/JsonConfig.h"

class ControlBaseConfig : public JsonConfig
{
  friend class RadioControlConfig;
public:
  explicit ControlBaseConfig() = default;

  ControlBaseConfig(const ControlBaseConfig& rhs) :
    m_type(rhs.m_type),
    m_config(rhs.m_config)
  {
  }
  ControlBaseConfig& operator=(const ControlBaseConfig& rhs)
  {
    m_type = rhs.m_type;
    m_config = rhs.m_config;
    return *this;
  }

  static ControlBaseConfig fromJson(const nlohmann::json& json)
  {
    ControlBaseConfig result;
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

#endif //CONTROLBASECONFIG_H
