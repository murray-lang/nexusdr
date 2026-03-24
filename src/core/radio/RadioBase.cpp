//
// Created by murray on 17/3/26.
//
#include "RadioBase.h"

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
  if (m_settings.applyUpdate(update)) {
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
    anyChanged |= m_settings.applyUpdate(updates[i]);
  }

  if (anyChanged) {
    applySettings(m_settings);
  }
}

void
RadioBase::notifyUpdate(const SettingUpdate& update)
{
  if (m_pEventTarget != nullptr) {
    EventDispatcher::postEvent(m_pEventTarget, new SettingUpdateEvent(update));
  }
}