//
// Created by murray on 5/08/25.
//

#ifndef CONTROLLERCONFIG_H
#define CONTROLLERCONFIG_H
#include "../../../config/JsonConfig.h"

class ControllerConfig : public JsonConfig
{
  friend class ControllersConfig;
public:
  explicit ControllerConfig() = default;

  ControllerConfig(const ControllerConfig& rhs) :
    m_type(rhs.m_type),
    m_config(rhs.m_config)
  {
  }
  ControllerConfig& operator=(const ControllerConfig& rhs)
  {
    m_type = rhs.m_type;
    m_config = rhs.m_config;
    return *this;
  }

  static ControllerConfig fromJson(const nlohmann::json& json)
  {
    ControllerConfig result;
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

#endif //CONTROLLERCONFIG_H
