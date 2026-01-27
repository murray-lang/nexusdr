//
// Created by murray on 2025-08-24.
//

#include "DigitalInput.h"
#include <settings/RadioSettings.h>

#include "../../../../../settings/core/SettingUpdateSink.h"

DigitalInput::DigitalInput() :
  GpioLines(Direction::INPUT),
  m_activeHigh(true),
  m_debounce(false),
  m_detectEdge(false),
  m_pSink(nullptr)
{
}

void
DigitalInput::configure(const DigitalInputConfig* pConfig)
{
  GpioLines::configure(pConfig);
  m_activeHigh = pConfig->getActiveHigh();
  m_debounce = pConfig->getDebounce();
  const std::string& strSettingPath = pConfig->getSettingPath();
  m_id = strSettingPath;
  m_settingPath = RadioSettings::getSettingUpdatePath(strSettingPath);
  setEdge(GpioLines::Edge::BOTH);
}

void
DigitalInput::connect(SettingUpdateSink* pSink)
{
  m_pSink = pSink;
}
void
DigitalInput::notifySettingUpdate(SettingUpdate& settingDelta)
{
  if (m_pSink) {
    m_pSink->applySettingUpdate(settingDelta);
  }
}

bool 
DigitalInput::handleLineChange(DigitalInputLinesRequest::LineStates& changedLines)
{
  DigitalInputLinesRequest::LineState& line = changedLines.at(m_lines[0]);
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
DigitalInput::notifyChange(const DigitalInputLinesRequest::LineState& lineState)
{
  bool active = m_activeHigh ? !!lineState.value : !lineState.value;
  SettingUpdate setting(m_settingPath, static_cast<bool>(active), SettingUpdate::VALUE);
  notifySettingUpdate(setting);
}
