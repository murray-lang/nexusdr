//
// Created by murray on 5/08/25.
//

#ifndef RECEIVERIQEVENT_H
#define RECEIVERIQEVENT_H
#include <memory>
#include <qcoreevent.h>
#include "../../SampleTypes.h"

class ReceiverIqEvent : public QEvent
{
public:
  static const QEvent::Type RxIqEvent;

  ReceiverIqEvent(vsdrcomplex& iq, uint32_t len):
    QEvent(RxIqEvent),
    buffer(new vsdrcomplex(iq.begin(), iq.begin() + len)),
    dataLength(len)
  {
  }

  std::shared_ptr<vsdrcomplex> buffer;
  uint32_t dataLength;

};

#endif //RECEIVERIQEVENT_H
