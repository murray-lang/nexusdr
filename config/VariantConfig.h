//
// Created by murray on 14/9/25.
//

#ifndef CUTESDR_VK6HL_VARIANTCONFIG_H
#define CUTESDR_VK6HL_VARIANTCONFIG_H
#include <string>
#include <nlohmann/json.hpp>

class VariantConfig
{
public:
  VariantConfig() = default;
  VariantConfig(const VariantConfig& rhs) = default;
  explicit VariantConfig(const nlohmann::json& json)
  {
    initialise(json);
  }
  ~VariantConfig() = default;

  VariantConfig& operator=(const VariantConfig& rhs) = default;
  VariantConfig& operator=(const nlohmann::json& json)
  {
    initialise(json);
    return *this;
  }


  void initialise(const nlohmann::json& json)
  {
    if (json.contains("type")) {
      m_type = json["type"];
    }
    if (json.contains("config")) {
      m_config = json["config"];
    }
  }

  [[nodiscard]] const std::string& getType() const
  {
    return m_type;
  }

  [[nodiscard]] const nlohmann::json& getConfig() const
  {
    return m_config;
  }

protected:
  std::string m_type;
  nlohmann::json m_config;
};

#endif //CUTESDR_VK6HL_VARIANTCONFIG_H