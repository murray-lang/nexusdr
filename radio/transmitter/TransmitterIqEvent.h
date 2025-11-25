//
// Created by murray on 22/11/25.
//

#pragma once


#include <memory>
#include <qcoreevent.h>
#include "../../SampleTypes.h"

class TransmitterIqEvent : public QEvent
{
public:
  static const QEvent::Type TxIqEvent;

  TransmitterIqEvent(const vsdrcomplex& iq, uint32_t len, uint32_t _sampleRate):
    QEvent(TxIqEvent),
    buffer(new vsdrcomplex(iq.begin(), iq.begin() + len)),
    dataLength(len),
    sampleRate(_sampleRate)
  {
  }

  std::shared_ptr<vsdrcomplex> buffer;
  uint32_t dataLength;
  uint32_t sampleRate;

};

