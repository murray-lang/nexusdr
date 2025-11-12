//
// Created by murray on 5/08/25.
//

#ifndef TRANSMITTERAUDIOEVENT_H
#define TRANSMITTERAUDIOEVENT_H
#include <memory>
#include <qcoreevent.h>
#include "../../SampleTypes.h"


class TransmitterAudioEvent : public QEvent
{
  public:
  static const QEvent::Type TxAudioEvent;

  TransmitterAudioEvent(vsdrreal& audio, uint32_t len):
    QEvent(TxAudioEvent),
    buffer(new vsdrreal(audio.begin(), audio.begin() + len)),
    dataLength(len)
  {
  }

  std::shared_ptr<vsdrreal> buffer;
  uint32_t dataLength;

};

#endif //TRANSMITTERAUDIOEVENT_H
