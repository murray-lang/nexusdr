//
// Created by murray on 25/9/25.
//

#pragma once

#include <memory>
#include <qcoreevent.h>

#include "RadioSettings.h"

class RadioSettingsEvent : public QEvent
{
public:
  static const QEvent::Type RadioSettingsEventType;

  explicit RadioSettingsEvent(const RadioSettings& settings) : QEvent(RadioSettingsEventType), m_settings(settings)
  {}

  [[nodiscard]] const RadioSettings& getSettings() const { return m_settings; }

protected:
  // std::shared_ptr<RadioSettings> m_pSettings;
  RadioSettings m_settings;
};
