//
// Created by murray on 25/9/25.
//

#pragma once
#include "RadioSettingsEvent.h"
#include "core/events/EventBase.h"


class SettingUpdateEvent : public EventBase
{
public:
  static const EventType SettingUpdateEventType;
  explicit SettingUpdateEvent(const SettingUpdate& setting);

  const SettingUpdate m_setting;
};
