//
// Created by murray on 2025-08-24.
//

#include "DigitalInput.h"
#include <settings/RadioSettings.h>

DigitalInput::DigitalInput() :
  GpioLines(Direction::INPUT),
  m_activeHigh(true),
  m_pSink(nullptr),
  m_detectEdge(false),
  m_debounce(false)
{

}

void
DigitalInput::configure(const DigitalInputConfig* pConfig)
{
  GpioLines::configure(pConfig);
  m_activeHigh = pConfig->getActiveHigh();
  m_debounce = pConfig->getDebounce();
  m_direction = Direction::INPUT;
  const std::string& strSettingPath = pConfig->getSettingPath();
  m_id = strSettingPath;
  m_settingPath = RadioSettings::getSettingPath(strSettingPath);
  setEdge(GpioLines::Edge::BOTH);
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

bool 
DigitalInput::handleLineChange(DigitalInputsRequest::LineStates& changedLines)
{
  DigitalInputsRequest::LineState& line = changedLines.at(m_lines[0]);
  if (!line.changed) {
    return false;
  }
  if (m_debounce && !line.isDebounced) {
    return false;
  }
 
  // qDebug() << "A:" << a.value << "B:" << b.value << "Direction: " << dir;
  notifyChange(line);
  line.changed = false; // TODO: Encapsulate this in LineState
  line.isDebounced = false;
  line.firstEdgeTime = 0;
  return true;
}

void
DigitalInput::notifyChange(const DigitalInputsRequest::LineState& lineState)
{
  bool active = m_activeHigh ? !!lineState.value : !lineState.value;
  SingleSetting setting(m_settingPath, static_cast<bool>(active), SingleSetting::VALUE);
  notifySingleSetting(setting);
}
