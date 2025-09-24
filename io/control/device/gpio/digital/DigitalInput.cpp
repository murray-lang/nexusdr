//
// Created by murray on 2025-08-24.
//

#include "DigitalInput.h"
#include <settings/RadioSettings.h>

DigitalInput::DigitalInput() :
  m_pSink(nullptr)
{

}

void
DigitalInput::configure(const DigitalInputConfig* pConfig)
{
  m_lines = pConfig->getLines();
  const std::string& strSettingPath = pConfig->getSettingPath();
  m_id = strSettingPath;
  m_settingPath = RadioSettings::getSettingPath(strSettingPath);
}

void
DigitalInput::connect(RadioSettingsSink* pSink)
{
  m_pSink = pSink;
}
void
DigitalInput::notifySingleSetting(const SettingDelta& settingDelta)
{
  if (m_pSink) {
    m_pSink->applySingleSetting(settingDelta);
  }
}
