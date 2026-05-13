//
// Created by murray on 5/08/25.
//

#include "../TransmitterAudioEvent.h"

const EventType TransmitterAudioEvent::TxAudioEvent = static_cast<EventType>(QEvent::registerEventType());

TransmitterAudioEvent::TransmitterAudioEvent(const RealSamplesMax& audio, uint32_t length, uint32_t sampleRate)
  : EventBase(TxAudioEvent)
  , buffer(new RealSamplesMax(audio.begin(), audio.begin() + length))
  , dataLength(length)
  , sampleRate(sampleRate)
{
}
