//
// Created by murray on 29/9/25.
//

#ifndef CUTESDR_VK6HL_GPIOLINECONFIG_H
#define CUTESDR_VK6HL_GPIOLINECONFIG_H
#include "ConfigBase.h"


class GpioLinesConfig : public ConfigBase
{
public:
  explicit GpioLinesConfig(const char* type) : ConfigBase(type) {}
  ~GpioLinesConfig() override = default;
  void fromJson(const nlohmann::json& json) override
  {
    if (json.contains("lines")) {
      for (auto& line : json["lines"]) {
        m_lines.push_back(line);
      }
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

  [[nodiscard]] const std::vector<uint32_t>& getLines() const { return m_lines; }
  [[nodiscard]] const std::string& getDirection() const { return m_direction; }
  [[nodiscard]] const std::string& getBias() const { return m_bias; }
  [[nodiscard]] const std::string& getEdge() const { return m_edge; }

protected:
  std::vector<uint32_t> m_lines;
  std::string m_direction; // "input" or "output"
  std::string m_bias; // "none", "pull-up" or "pull-down"
  std::string m_edge; // "rising", "falling" or "both"
};
#endif //CUTESDR_VK6HL_GPIOLINECONFIG_H