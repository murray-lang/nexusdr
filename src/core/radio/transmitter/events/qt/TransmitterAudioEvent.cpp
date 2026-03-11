//
// Created by murray on 5/08/25.
//

#include "../TransmitterAudioEvent.h"

const EventType TransmitterAudioEvent::TxAudioEvent = static_cast<EventType>(QEvent::registerEventType());

TransmitterAudioEvent::TransmitterAudioEvent(const vsdrreal& audio, uint32_t len):
    EventBase(TxAudioEvent),
    buffer(new vsdrreal(audio.begin(), audio.begin() + len)),
    dataLength(len)
{
}
