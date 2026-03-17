//
// Created by murray on 5/08/25.
//

#pragma once

#include <memory>
#include "core/SampleTypes.h"
#include "core/events/EventBase.h"
#include "core/events/EventType.h"


class ReceiverAudioEvent : public EventBase
{
public:
  static const EventType RxAudioEvent;
  ReceiverAudioEvent(const vsdrreal& audio, uint32_t len);

  std::shared_ptr<vsdrreal> buffer;
  uint32_t dataLength;

};
