//
// Created by murray on 2025-08-24.
//

#include "DigitalInput.h"
#include "../../../../RadioSettings.h"

void
DigitalInput::initialise(const nlohmann::json& json)
{
  if (json.contains("lines")) {
    m_lines = json["lines"].get<std::vector<uint32_t>>();
  }
  if (json.contains("settingPath")) {
    std::string strSettingPath = json["settingPath"];
    m_id = strSettingPath;
    m_settingPath = RadioSettings::getSettingPath(strSettingPath);
  }
}
