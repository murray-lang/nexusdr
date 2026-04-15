//
// Created by murray on 22/11/25.
//

#include "../TransmitterIqEvent.h"

const EventType TransmitterIqEvent::TxIqEvent = static_cast<EventType>(QEvent::registerEventType());

TransmitterIqEvent::TransmitterIqEvent(const ComplexSamplesMax& iq, uint32_t len, uint32_t _sampleRate):
    EventBase(TxIqEvent),
    buffer(new ComplexSamplesMax(iq.begin(), iq.begin() + len)),
    dataLength(len),
    sampleRate(_sampleRate)
{
}