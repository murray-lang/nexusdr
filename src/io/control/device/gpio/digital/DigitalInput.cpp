#include "DigitalInput.h"
#include "core/config-settings/settings/RadioSettings.h"

#include "core/config-settings/settings/base/SettingUpdateSink.h"

DigitalInput::DigitalInput() :
  GpioLines(Direction::INPUT),
  m_activeHigh(true),
  m_debounce(false),
  m_detectEdge(false)
{
}

ResultCode
DigitalInput::configure(const Config::DigitalInput::Fields& config)
{
  ResultCode rc = GpioLines::configureLines(config);
  if (rc != ResultCode::OK) return rc;

  m_activeHigh = config.activeHigh;
  m_debounce = config.debounce;
  const SettingPathString& strSettingPath = config.settingPath;
  m_id = strSettingPath;
  setEdge(GpioLines::Edge::BOTH);
  return RadioSettings::getSettingUpdatePath(strSettingPath, m_settingPath);
}

void
DigitalInput::connectSettingUpdateSink(SettingUpdateSink& pSink)
{
  m_pSink.emplace(pSink);
}
ResultCode
DigitalInput::notifySettingUpdate(SettingUpdate& settingDelta)
{
  if (m_pSink) {
    m_pSink->get().applySettingUpdate(settingDelta);
  }
  return ResultCode::OK;
}

bool 
DigitalInput::handleLineChange(DigitalInputLinesRequest::LineStateVector& changedLines)
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
