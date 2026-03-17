//
// Created by murray on 25/9/25.
//

#include "../SettingUpdateEvent.h"

const EventType SingleSettingEventType = static_cast<QEvent::Type>(QEvent::registerEventType());

SettingUpdateEvent::SettingUpdateEvent(const SettingUpdate& setting) :
  EventBase(SingleSettingEventType),
  m_setting(setting)
{}