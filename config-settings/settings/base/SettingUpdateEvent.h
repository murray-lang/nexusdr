//
// Created by murray on 25/9/25.
//

#ifndef CUTESDR_VK6HL_SINGLESETTINGEVENT_H
#define CUTESDR_VK6HL_SINGLESETTINGEVENT_H
#include "../RadioSettingsEvent.h"


class SettingUpdateEvent : public QEvent
{
public:
  static const QEvent::Type SingleSettingEventType;
  explicit SettingUpdateEvent(const SettingUpdate& setting) : QEvent(SingleSettingEventType), m_setting(setting)
  {}

  const SettingUpdate m_setting;
};


#endif //CUTESDR_VK6HL_SINGLESETTINGEVENT_H