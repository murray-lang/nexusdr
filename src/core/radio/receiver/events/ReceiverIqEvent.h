//
// Created by murray on 5/08/25.
//

#pragma once

#include <memory>
#include <qcoreevent.h>
#include "core/SampleTypes.h"
#include "core/events/EventBase.h"
#include "core/events/EventType.h"

class ReceiverIqEvent : public EventBase
{
public:
  static const EventType RxIqEvent;
  ReceiverIqEvent(const ComplexSamplesMax& iq, uint32_t len, uint32_t _sampleRate);

  std::shared_ptr<ComplexSamplesMax> buffer;
  uint32_t dataLength;
  uint32_t sampleRate;
};

