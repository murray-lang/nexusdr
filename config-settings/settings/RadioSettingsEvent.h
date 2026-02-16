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

  explicit RadioSettingsEvent(const RadioSettings& radioSettings, uint64_t sequence) :
    QEvent(RadioSettingsEventType),
    // m_pRadioSettings(new RadioSettings(radioSettings)),
    // m_pBandSettings(new BandSettings(bandSettings)),
    m_radioSettings(radioSettings),
    m_sequence(sequence)
  {}
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
