//
// Created by murray on 5/08/25.
//

#pragma once

#include <memory>
#include <qcoreevent.h>
#include "../../SampleTypes.h"

class ReceiverIqEvent : public QEvent
{
public:
  static const QEvent::Type RxIqEvent;

  ReceiverIqEvent(const vsdrcomplex& iq, uint32_t len, uint32_t _sampleRate):
    QEvent(RxIqEvent),
    buffer(new vsdrcomplex(iq.begin(), iq.begin() + len)),
    dataLength(len),
    sampleRate(_sampleRate)
  {
  }

  std::shared_ptr<vsdrcomplex> buffer;
  uint32_t dataLength;
  uint32_t sampleRate;

};

