#include "QtMeteringDispatcher.h"

#include "core/events/EventDispatcher.h"
#include "core/radio/receiver/events/ReceiverMeterEvent.h"

QtMeteringDispatcher::QtMeteringDispatcher(function<QObject*()> eventTargetProvider)
: m_eventTargetProvider(move(eventTargetProvider))
{
}

void
QtMeteringDispatcher::updateRxMetering(const IqReceiverMetering& metering)
{
  QObject* pEventTarget = m_eventTargetProvider();
  if (pEventTarget != nullptr) {
    EventDispatcher::postEvent(pEventTarget, new ReceiverMeterEvent(metering));
  }

}