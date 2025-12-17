//
// Created by murray on 27/07/25.
//

#pragma once
#include <nlohmann/json.hpp>
#include <string>

class ConfigBase
{
public:
  explicit ConfigBase(const char* type) : m_type(type) {}
  ConfigBase(const ConfigBase& rhs) = default;
  virtual ~ConfigBase() = default;

  // ConfigBase& operator=(const ConfigBase& rhs) = default;

  // Load this configuration from JSON (symmetrical with toJson()).
  // All derived classes must implement this.
  virtual void fromJson(const nlohmann::json& json) = 0;

  // Serialize this configuration back to JSON. Default: empty object
  // Derived classes should override to include their fields.
  [[nodiscard]] virtual nlohmann::json toJson() const { return nlohmann::json::object(); }

  [[nodiscard]] const std::string& getType() const { return m_type; }
protected:
  std::string m_type;
};
