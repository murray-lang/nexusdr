//
// Created by murray on 5/08/25.
//

#pragma once

#include <memory>
#include <qcoreevent.h>
#include "core/SampleTypes.h"
#include "core/events/EventBase.h"
#include "core/events/EventType.h"


class TransmitterAudioEvent : public EventBase
{
public:
  static const EventType TxAudioEvent;
  TransmitterAudioEvent(const RealSamplesMax& audio, uint32_t length, uint32_t sampleRate);

  std::shared_ptr<RealSamplesMax> buffer;
  uint32_t dataLength;
  uint32_t sampleRate;
};
