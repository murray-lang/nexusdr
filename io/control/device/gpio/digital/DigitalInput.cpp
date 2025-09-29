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
  m_lines.clear();
  const std::vector<uint32_t>& lineNos = pConfig->getLines();
  for (auto lineNo : lineNos) {
    GpioLine line(lineNo, pConfig);
    line.setDirection(GpioLine::Direction::INPUT);
    m_lines.emplace_back(line);
  }
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
