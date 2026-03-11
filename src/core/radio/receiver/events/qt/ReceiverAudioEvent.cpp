//
// Created by murray on 5/08/25.
//

#include "../ReceiverAudioEvent.h"

const EventType ReceiverAudioEvent::RxAudioEvent = static_cast<EventType>(QEvent::registerEventType());

ReceiverAudioEvent::ReceiverAudioEvent(const vsdrreal& audio, uint32_t len):
    EventBase(RxAudioEvent),
    buffer(new vsdrreal(audio.begin(), audio.begin() + len)),
    dataLength(len)
{
}
