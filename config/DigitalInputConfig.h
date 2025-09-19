//
// Created by murray on 15/9/25.
//

#ifndef CUTESDR_VK6HL_DIGITALINPUTCONFIG_H
#define CUTESDR_VK6HL_DIGITALINPUTCONFIG_H
#include "ConfigBase.h"
#include "ConfigException.h"

class DigitalInputConfig : public ConfigBase
{
public:
  static constexpr auto type = "digitalinput";

  DigitalInputConfig() : ConfigBase(type), m_debounce(true) {}
  explicit DigitalInputConfig(const char * subtype) : ConfigBase(subtype) {}
  ~DigitalInputConfig() override  = default;

  void initialise(const nlohmann::json& json) override
  {
    if (json.contains("debounce")) {
      m_debounce = json["debounce"];
    }
    if (json.contains("lines")) {
      for (auto& line : json["lines"]) {
        m_lines.push_back(line);
      }
    }
    if (m_lines.empty()) {
      throw ConfigException("DigitalInputConfig: lines empty");
    }
    if (json.contains("settingPath")) {
      m_settingPath = json["settingPath"];
    } else {
      throw ConfigException("DigitalInputConfig: settingPath empty");
    }
  }

  const std::vector<uint32_t>& getLines() const { return m_lines; }
  const std::string& getSettingPath() const { return m_settingPath; }

protected:
  std::vector<uint32_t> m_lines;
  std::string m_settingPath;
  bool m_debounce;
};
#endif //CUTESDR_VK6HL_DIGITALINPUTCONFIG_H