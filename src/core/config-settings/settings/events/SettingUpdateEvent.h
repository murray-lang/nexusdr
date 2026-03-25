//
// Created by murray on 25/9/25.
//

#pragma once
#include "RadioSettingsEvent.h"
#include "SettingEventBase.h"


class SettingUpdateEvent : public SettingEventBase
{
public:
  static const EventType SettingUpdateEventType;
  explicit SettingUpdateEvent(const SettingUpdate& setting, EventSource source);

  const SettingUpdate m_setting;
};
