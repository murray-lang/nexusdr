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
  TransmitterAudioEvent(const vsdrreal& audio, uint32_t len);

  std::shared_ptr<vsdrreal> buffer;
  uint32_t dataLength;

};
