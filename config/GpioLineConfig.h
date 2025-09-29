//
// Created by murray on 29/9/25.
//

#ifndef CUTESDR_VK6HL_GPIOLINECONFIG_H
#define CUTESDR_VK6HL_GPIOLINECONFIG_H
#include "ConfigBase.h"


class GpioLineConfig : public ConfigBase
{
public:
  explicit GpioLineConfig(const char* type) : ConfigBase(type), m_line(0) {}
  ~GpioLineConfig() override = default;
  void initialise(const nlohmann::json& json) override
  {
    if (json.contains("line")) {
      m_line = json["line"];
    }
    if (json.contains("direction")) {
      m_direction = json["direction"];
    }
    if (json.contains("bias")) {
      m_bias = json["bias"];
    }
    if (json.contains("edge")) {
      m_edge = json["edge"];
    }
  }

  [[nodiscard]] uint32_t getLine() const { return m_line; }
  [[nodiscard]] const std::string& getDirection() const { return m_direction; }
  [[nodiscard]] const std::string& getBias() const { return m_bias; }
  [[nodiscard]] const std::string& getEdge() const { return m_edge; }

protected:
  uint32_t m_line;
  std::string m_direction; // "input" or "output"
  std::string m_bias; // "none", "pull-up" or "pull-down"
  std::string m_edge; // "rising", "falling" or "both"
};
#endif //CUTESDR_VK6HL_GPIOLINECONFIG_H