//
// Created by murray on 25/9/25.
//

#include "../RadioSettingsEvent.h"

const EventType RadioSettingsEvent::RadioSettingsEventType = static_cast<EventType>(QEvent::registerEventType());

RadioSettingsEvent::RadioSettingsEvent(const RadioSettings& radioSettings, uint64_t sequence, EventSource source) :
    SettingEventBase(RadioSettingsEventType, source),
    // m_pRadioSettings(new RadioSettings(radioSettings)),
    // m_pBandSettings(new BandSettings(bandSettings)),
    m_radioSettings(radioSettings),
    m_sequence(sequence)
{}