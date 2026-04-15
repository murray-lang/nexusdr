//
// Created by murray on 5/08/25.
//

#include "../TransmitterAudioEvent.h"

const EventType TransmitterAudioEvent::TxAudioEvent = static_cast<EventType>(QEvent::registerEventType());

TransmitterAudioEvent::TransmitterAudioEvent(const RealSamplesMax& audio, uint32_t len):
    EventBase(TxAudioEvent),
    buffer(new RealSamplesMax(audio.begin(), audio.begin() + len)),
    dataLength(len)
{
}
