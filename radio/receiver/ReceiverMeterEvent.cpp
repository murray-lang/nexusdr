//
// Created by murray on 17/2/26.
//

#include "ReceiverMeterEvent.h"
const QEvent::Type ReceiverMeterEvent::RxMeterEvent =
  static_cast<QEvent::Type>(QEvent::registerEventType());