//
// Created by murray on 27/07/25.
//

#ifndef CUTESDR_VK6HL_CONFIGBASE_H
#define CUTESDR_VK6HL_CONFIGBASE_H
#include <nlohmann/json.hpp>

class ConfigBase
{
public:
  explicit ConfigBase(const char* type) : m_type(type) {}
  // ConfigBase(const ConfigBase& rhs) = default;
  virtual ~ConfigBase() = default;

  // ConfigBase& operator=(const ConfigBase& rhs) = default;

  // static ConfigBase fromJson(const nlohmann::json& json) { return {}; }
  virtual void initialise(const nlohmann::json& json) = 0;

  [[nodiscard]] const std::string& getType() const { return m_type; }
protected:
  std::string m_type;
};
#endif //CUTESDR_VK6HL_CONFIGBASE_H
