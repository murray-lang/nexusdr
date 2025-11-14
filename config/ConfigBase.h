//
// Created by murray on 27/07/25.
//

#ifndef CUTESDR_VK6HL_CONFIGBASE_H
#define CUTESDR_VK6HL_CONFIGBASE_H
#include <nlohmann/json.hpp>
#include <string>

class ConfigBase
{
public:
  explicit ConfigBase(const char* type) : m_type(type) {}
  // ConfigBase(const ConfigBase& rhs) = default;
  virtual ~ConfigBase() = default;

  // ConfigBase& operator=(const ConfigBase& rhs) = default;

  // Load this configuration from JSON (symmetrical with toJson()).
  // All derived classes must implement this.
  virtual void fromJson(const nlohmann::json& json) = 0;

  // Serialize this configuration back to JSON. Default: empty object
  // Derived classes should override to include their fields.
  [[nodiscard]] virtual nlohmann::json toJson() const { return nlohmann::json::object(); }

  // Provide a self-documenting description/schema of this configuration.
  // Default: returns an object with the config type only.
  [[nodiscard]] virtual nlohmann::json describe() const { return nlohmann::json{{"type", m_type}}; }

  [[nodiscard]] const std::string& getType() const { return m_type; }
protected:
  std::string m_type;
};
#endif //CUTESDR_VK6HL_CONFIGBASE_H
