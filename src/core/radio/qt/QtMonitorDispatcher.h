#pragma once

#include "CrossPlatformTypes.h"
#include "core/radio/MonitorSink.h"
#include <QObject>

class QtMonitorDispatcher: public MonitorSink
{
public:
  QtMonitorDispatcher(function<QObject*()> eventTargetProvider);
  ~QtMonitorDispatcher() override = default;

  void updateRxIq(const ComplexSamplesMax& iq, uint32_t length, uint32_t sampleRate) override;
  void updateTxIq(const ComplexSamplesMax& iq, uint32_t length, uint32_t sampleRate) override;

  void updateRxAudio(const RealSamplesMax& audio, uint32_t length, uint32_t sampleRate) override;
  void updateTxAudio(const RealSamplesMax& audio, uint32_t length, uint32_t sampleRate) override;

private:
  function<QObject*()> m_eventTargetProvider;
};
