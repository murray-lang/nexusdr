//
// Created by murray on 2025-08-24.
//

#include "DigitalInput.h"
#include <settings/RadioSettings.h>

void
DigitalInput::configure(const DigitalInputConfig* pConfig)
{
  m_lines = pConfig->getLines();
  std::string strSettingPath = pConfig->getSettingPath();
  m_id = strSettingPath;
  m_settingPath = RadioSettings::getSettingPath(strSettingPath);
}
