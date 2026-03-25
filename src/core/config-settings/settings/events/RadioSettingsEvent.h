//
// Created by murray on 25/9/25.
//

#pragma once

#include <memory>

#include "SettingEventBase.h"
#include "../RadioSettings.h"
#include "core/events/EventType.h"

class RadioSettingsEvent : public SettingEventBase
{
public:
  static const EventType RadioSettingsEventType;
  explicit RadioSettingsEvent(const RadioSettings& radioSettings, uint64_t sequence, EventSource source);
  // [[nodiscard]] const RadioSettings* getRadioSettings() const { return m_pRadioSettings.get(); }
  // [[nodiscard]] const BandSettings* getBandSettings() const { return m_pBandSettings.get(); }
  [[nodiscard]] const RadioSettings& getRadioSettings() const { return m_radioSettings; }
  [[nodiscard]] uint64_t getSequence() const { return m_sequence; }



protected:
  // std::shared_ptr<RadioSettings> m_pRadioSettings;
  // std::shared_ptr<BandSettings> m_pBandSettings;
  RadioSettings m_radioSettings;
  uint64_t m_sequence;
};
