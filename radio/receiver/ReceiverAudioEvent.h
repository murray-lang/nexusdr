//
// Created by murray on 5/08/25.
//

#ifndef RECEIVERAUDIOEVENT_H
#define RECEIVERAUDIOEVENT_H
#include <memory>
#include <qcoreevent.h>
#include "../../SampleTypes.h"


class ReceiverAudioEvent : public QEvent
{
  public:
  static const QEvent::Type RxAudioEvent;

  ReceiverAudioEvent(vsdrreal& audio, uint32_t len):
    QEvent(RxAudioEvent),
    buffer(new vsdrreal(audio.begin(), audio.begin() + len)),
    dataLength(len)
  {
  }

  std::shared_ptr<vsdrreal> buffer;
  uint32_t dataLength;

};


#endif //RECEIVERAUDIOEVENT_H
