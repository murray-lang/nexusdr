//
// Created by murray on 22/11/25.
//

#pragma once


#include <memory>
#include "core/SampleTypes.h"
#include "core/events/EventBase.h"
#include "core/events/EventType.h"

class TransmitterIqEvent : public EventBase
{
public:
  static const EventType TxIqEvent;
  TransmitterIqEvent(const vsdrcomplex& iq, uint32_t len, uint32_t _sampleRate);

  std::shared_ptr<vsdrcomplex> buffer;
  uint32_t dataLength;
  uint32_t sampleRate;
};

