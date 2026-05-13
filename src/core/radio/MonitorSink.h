#pragma once
#include "core/SampleTypes.h"

class MonitorSink
{
public:
  virtual ~MonitorSink() = default;
  virtual void updateRxIq(const ComplexSamplesMax& iq, uint32_t length, uint32_t sampleRate) = 0;
  virtual void updateTxIq(const ComplexSamplesMax& iq, uint32_t length, uint32_t sampleRate) = 0;

  virtual void updateRxAudio(const RealSamplesMax& audio, uint32_t length, uint32_t sampleRate) = 0;
  virtual void updateTxAudio(const RealSamplesMax& audio, uint32_t length, uint32_t sampleRate) = 0;
};
