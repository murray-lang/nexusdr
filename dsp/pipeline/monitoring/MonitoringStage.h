//
// Created by murray on 27/07/25.
//

#pragma once

#include "../IqPipelineStage.h"
// #include "../../../SignalEmitter.h"
#include <qcoreevent.h>
#include <qcoreapplication.h>

#include "radio/receiver/ReceiverAudioEvent.h"
#include "radio/receiver/ReceiverIqEvent.h"
#include "radio/transmitter/TransmitterAudioEvent.h"
#include "radio/transmitter/TransmitterIqEvent.h"

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
      QCoreApplication::postEvent(m_eventTarget, new ReceiverIqEvent(samples, inputLength, m_sampleRateProvider()));
    } else if (m_type == TransmitterIqEvent::TxIqEvent) {
      QCoreApplication::postEvent(m_eventTarget, new TransmitterIqEvent(samples, inputLength, m_sampleRateProvider()));
    }
    buffers.flip();
    return inputLength;
  }

protected:
  QEvent::Type m_type;
  QObject* m_eventTarget;
  std::function<uint32_t()> m_sampleRateProvider;
};
