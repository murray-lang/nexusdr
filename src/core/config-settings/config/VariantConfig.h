//
// Created by murray on 14/9/25.
//

#pragma once

#include <string>
#include <ArduinoJson.h>

class VariantConfig
{
public:
  VariantConfig() = default;
  VariantConfig(const VariantConfig& rhs) : m_type(rhs.m_type) {
    m_config.set(rhs.m_config.as<JsonVariantConst>());
  }
  explicit VariantConfig(JsonVariantConst json)
  {
    fromJson(json);
  }
  ~VariantConfig() = default;

  VariantConfig& operator=(const VariantConfig& rhs)
  {
    if (this != &rhs) {
      m_type = rhs.m_type;
      m_config.set(rhs.m_config.as<JsonVariantConst>());
    }
    return *this;
  }

  VariantConfig& operator=(JsonVariantConst json)
  {
    fromJson(json);
    return *this;
  }

  void fromJson(JsonVariantConst json)
  {
    if (json["type"]) {
      m_type = json["type"].as<const char *>();
    }
    if (json["config"]) {
      m_config.set(json["config"]);  // Deep copy into document
    }
  }

  void setType(const std::string& type)
  {
    m_type = type;
  }

  [[nodiscard]] const std::string& getType() const
  {
    return m_type;
  }

  [[nodiscard]] JsonVariantConst getConfig() const
  {
    return m_config.as<JsonVariantConst>();
  }

  void toJson(JsonObject& json) const
  {
    json["type"] = m_type;
    json["config"] = m_config;
  }

protected:
  std::string m_type;
  JsonDocument m_config;  // Uses stack allocation by default in v7
};
