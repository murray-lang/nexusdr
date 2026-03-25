//
// Created by murray on 24/3/26.
//

#pragma once
#include "core/events/EventBase.h"
#include "core/events/EventType.h"

class SettingEventBase : public EventBase
{
public:
  enum EventSource { ANY, FRONT_END, BACK_END };

  SettingEventBase(const EventType type, const EventSource source) : EventBase(type), m_source(source) {}
  ~SettingEventBase() override = default;

  [[nodiscard]] EventSource getSource() const { return m_source; }

protected:
  EventSource m_source;

};
