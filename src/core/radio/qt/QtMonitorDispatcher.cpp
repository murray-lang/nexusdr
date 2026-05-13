#include "QtMonitorDispatcher.h"

#include "core/events/EventDispatcher.h"
#include "core/radio/receiver/events/ReceiverAudioEvent.h"
#include "core/radio/receiver/events/ReceiverIqEvent.h"
#include "core/radio/transmitter/events/TransmitterAudioEvent.h"
#include "core/radio/transmitter/events/TransmitterIqEvent.h"

QtMonitorDispatcher::QtMonitorDispatcher(function<QObject*()> eventTargetProvider)
  : m_eventTargetProvider(move(eventTargetProvider))
{
}

void
QtMonitorDispatcher::updateRxIq(const ComplexSamplesMax& iq, uint32_t length, uint32_t sampleRate)
{
  QObject* pEventTarget = m_eventTargetProvider();
  if (pEventTarget != nullptr) {
    EventDispatcher::postEvent(pEventTarget, new ReceiverIqEvent(iq, length, sampleRate));
  }
}

void
QtMonitorDispatcher::updateTxIq(const ComplexSamplesMax& iq, uint32_t length, uint32_t sampleRate)
{
  QObject* pEventTarget = m_eventTargetProvider();
  if (pEventTarget != nullptr) {
    EventDispatcher::postEvent(pEventTarget, new TransmitterIqEvent(iq, length, sampleRate));
  }

}

void
QtMonitorDispatcher::updateRxAudio(const RealSamplesMax& audio, uint32_t length, uint32_t sampleRate)
{
  QObject* pEventTarget = m_eventTargetProvider();
  if (pEventTarget != nullptr) {
    EventDispatcher::postEvent(pEventTarget, new ReceiverAudioEvent(audio, length, sampleRate));
  }
}

void
QtMonitorDispatcher::updateTxAudio(const RealSamplesMax& audio, uint32_t length, uint32_t sampleRate)
{
  QObject* pEventTarget = m_eventTargetProvider();
  if (pEventTarget != nullptr) {
    EventDispatcher::postEvent(pEventTarget, new TransmitterAudioEvent(audio, length, sampleRate));
  }
}