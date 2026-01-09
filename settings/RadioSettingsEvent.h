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

  explicit RadioSettingsEvent(const RadioSettings& radioSettings, const BandSettings& bandSettings) :
    QEvent(RadioSettingsEventType),
    m_radioSettings(radioSettings),
    m_bandSettings(bandSettings)
  {}

  [[nodiscard]] const RadioSettings& getRadioSettings() const { return m_radioSettings; }
  [[nodiscard]] const BandSettings& getBandSettings() const { return m_bandSettings; }

protected:
  // std::shared_ptr<RadioSettings> m_pSettings;
  RadioSettings m_radioSettings;
  BandSettings m_bandSettings;
};
