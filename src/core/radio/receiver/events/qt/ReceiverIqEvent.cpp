//
// Created by murray on 5/08/25.
//
#include "../ReceiverIqEvent.h"

const EventType ReceiverIqEvent::RxIqEvent = static_cast<EventType>(QEvent::registerEventType());

ReceiverIqEvent::ReceiverIqEvent(const ComplexSamplesMax& iq, uint32_t len, uint32_t _sampleRate):
    EventBase(RxIqEvent),
    buffer(new ComplexSamplesMax(iq.begin(), iq.begin() + len)),
    dataLength(len),
    sampleRate(_sampleRate)
{
}