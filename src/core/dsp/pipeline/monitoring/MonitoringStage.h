//
// Created by murray on 27/07/25.
//

#pragma once

#include "../IqPipelineStage.h"

#include "core/radio/receiver/events/ReceiverAudioEvent.h"
#include "core/radio/receiver/events/ReceiverIqEvent.h"
#include "core/radio/transmitter/events/TransmitterAudioEvent.h"
#include "core/radio/transmitter/events/TransmitterIqEvent.h"
#include "core/events/EventDispatcher.h"
#include "core/radio/MonitorSource.h"

class MonitoringStage : public IqPipelineStage, public MonitorSource
{
public:
  explicit MonitoringStage(MonitorSink* pMonitorSink, QEvent::Type type, function<uint32_t()> sampleRateProvider)
    : MonitorSource(pMonitorSink)
    , m_type(type)
    , m_sampleRateProvider(move(sampleRateProvider))
  {

  }
  uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) override
  {
    ComplexSamplesMax& samples = buffers.input();
    if (m_pSink != nullptr) {
      if (m_type == ReceiverIqEvent::RxIqEvent) {
        notifyRxIq(samples, inputLength, m_sampleRateProvider());
        // EventDispatcher::postEvent(m_eventTarget, new ReceiverIqEvent(samples, inputLength, m_sampleRateProvider()));
      } else if (m_type == TransmitterIqEvent::TxIqEvent) {
        notifyTxIq(samples, inputLength, m_sampleRateProvider());
        // EventDispatcher::postEvent(m_eventTarget, new TransmitterIqEvent(samples, inputLength, m_sampleRateProvider()));
      }
    }
    buffers.flip();
    return inputLength;
  }

protected:
  EventType m_type;
  function<uint32_t()> m_sampleRateProvider;
};
