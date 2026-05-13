//
// Created by murray on 17/2/26.
//

#include "../ReceiverMeterEvent.h"
#include <QEvent>

const EventType ReceiverMeterEvent::RxMeterEvent = static_cast<EventType>(QEvent::registerEventType());

ReceiverMeterEvent::ReceiverMeterEvent(float rssiDbFs, uint32_t sampleRate, std::optional<float> agcGainDb)
  : EventBase(RxMeterEvent)
  , m_metering{rssiDbFs, sampleRate, agcGainDb}
{
}

ReceiverMeterEvent::ReceiverMeterEvent(const IqReceiverMetering& metering)
  : EventBase(RxMeterEvent)
  , m_metering{metering}
{

}