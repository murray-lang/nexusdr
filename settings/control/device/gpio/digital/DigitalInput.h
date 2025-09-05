//
// Created by murray on 2025-08-24.
//

#ifndef CUTESDR_VK6HL_DIGITALINPUT_H
#define CUTESDR_VK6HL_DIGITALINPUT_H
#include <cstdint>
#include <vector>
#include <nlohmann/json.hpp>
#include "settings/SettingPath.h"
#include "../GpioLines.h"


class DigitalInput
{
public:
  explicit DigitalInput(GpioLines& lines) {}
  virtual ~DigitalInput() = default;

  virtual void initialise(const nlohmann::json& json);
  // void setId(const std::string& id) { m_id = id; }
  [[nodiscard]] const std::string& getId() const { return m_id; }
  [[nodiscard]] const std::vector<uint32_t>& getLines() const { return m_lines; }
  [[nodiscard]] const SettingPath& getSettingPath() const { return m_settingPath; }

  virtual bool handleLineChange(GpioLines::LineStateMap& changedLines) = 0;

protected:
  std::string m_id;
  std::vector<uint32_t> m_lines;
  SettingPath m_settingPath;
};


#endif //CUTESDR_VK6HL_DIGITALINPUT_H