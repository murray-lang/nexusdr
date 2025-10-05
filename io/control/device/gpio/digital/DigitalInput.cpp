//
// Created by murray on 2025-08-24.
//

#include "DigitalInput.h"
#include <settings/RadioSettings.h>

DigitalInput::DigitalInput() :
  GpioLines(Direction::INPUT),
  m_pSink(nullptr),
  m_detectEdge(false),
  m_debounce(false)
{

}

void
DigitalInput::configure(const DigitalInputConfig* pConfig)
{
  GpioLines::configure(pConfig);
  m_debounce = pConfig->getDebounce();
  m_direction = Direction::INPUT;
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
DigitalInput::notifySingleSetting(const SingleSetting& settingDelta)
{
  if (m_pSink) {
    m_pSink->applySingleSetting(settingDelta);
  }
}
