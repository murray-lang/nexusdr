//
// Created by murray on 17/3/26.
//
#include "RadioBase.h"

#include "core/config-settings/settings/base/SettingUpdateHelpers.h"
#include "core/config-settings/settings/events/SettingUpdateEvent.h"
#include "core/events/EventDispatcher.h"

RadioBase::RadioBase(EventTarget *pEventTarget) :
     m_pEventTarget(pEventTarget)
    , m_settings()
    ,m_updateSequenceNo(0)
{

}

void
RadioBase::applySettingUpdate(SettingUpdate& update)
{
  if (m_settings.applyUpdate(update, m_bandSelector)) {
    applySettings(m_settings);
  }
}

void
RadioBase::applySettingUpdates(SettingUpdate* updates, std::size_t count)
{
  if (!updates) return;

  bool anyChanged = false;

  for (std::size_t i = 0; i < count; ++i) {
    updates[i].resetCursor();
    anyChanged |= m_settings.applyUpdate(updates[i], m_bandSelector);
  }

  if (anyChanged) {
    applySettings(m_settings);
  }
}

void
RadioBase::notifyUpdate(const SettingUpdate& update, SettingEventBase::EventSource source)
{
  if (m_pEventTarget != nullptr) {
    EventDispatcher::postEvent(m_pEventTarget, new SettingUpdateEvent(update, source));
  }
}

void
RadioBase::applyBand(const std::string& bandName)
{
  SettingUpdate update = SettingUpdateHelpers::makeSetBand(bandName);
  // qDebug() << "Radio::applyBand(): applying band " << bandName.c_str() << ". Existing band: " << m_settings.bandName.c_str() ;
  applySettingUpdate(update);
}

void
RadioBase::split(const std::string& bandA, const std::string& bandB)
{
  SettingUpdatePath splitPath({RadioSettings::BAND, ActiveBandSettings::SPLIT});
  SettingUpdate splitSetting(splitPath, true, SettingUpdate::Meaning::VALUE);
  applySettingUpdate(splitSetting);
}

void
RadioBase::applyAgcSpeed(AgcSpeed speed)
{
  SettingUpdate update = SettingUpdateHelpers::makeSetAgcSpeedOnFocusPipeline(speed);
  applySettingUpdate(update);
}