//
// Created by murray on 5/08/25.
//

#include "../ReceiverAudioEvent.h"

const EventType ReceiverAudioEvent::RxAudioEvent = static_cast<EventType>(QEvent::registerEventType());

ReceiverAudioEvent::ReceiverAudioEvent(const RealSamplesMax& audio, uint32_t length, uint32_t sampleRate)
  : EventBase(RxAudioEvent)
  , buffer(new RealSamplesMax(audio.begin(), audio.begin() + length))
  , dataLength(length)
  , sampleRate(sampleRate)
{
}
