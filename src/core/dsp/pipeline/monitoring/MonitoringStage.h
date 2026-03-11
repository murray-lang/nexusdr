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

class MonitoringStage : public IqPipelineStage
{
public:
  explicit MonitoringStage(QObject* eventTarget, QEvent::Type type, std::function<uint32_t()> sampleRateProvider) :
    m_type(type),
    m_eventTarget(eventTarget),
    m_sampleRateProvider(std::move(sampleRateProvider))
  {

  }
  uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) override
  {
    vsdrcomplex& samples = buffers.input();
    if (m_type == ReceiverIqEvent::RxIqEvent) {
      EventDispatcher::postEvent(m_eventTarget, new ReceiverIqEvent(samples, inputLength, m_sampleRateProvider()));
    } else if (m_type == TransmitterIqEvent::TxIqEvent) {
      EventDispatcher::postEvent(m_eventTarget, new TransmitterIqEvent(samples, inputLength, m_sampleRateProvider()));
    }
    buffers.flip();
    return inputLength;
  }

protected:
  EventType m_type;
  EventTarget* m_eventTarget;
  std::function<uint32_t()> m_sampleRateProvider;
};
