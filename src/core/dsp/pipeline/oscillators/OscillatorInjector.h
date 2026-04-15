#pragma once

#include "OscillatorStage.h"
#include <algorithm>

class OscillatorInjector : public OscillatorStage
{
public:
  OscillatorInjector() : OscillatorStage() {}
  OscillatorInjector(uint32_t sampleRate, int32_t frequency) : OscillatorStage(sampleRate, frequency) {}
  ~OscillatorInjector() override = default;

  uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) override
  {
    const ComplexSamplesMax& input = buffers.input();
    ComplexSamplesMax& output = buffers.output();
    for (uint32_t i = 0; i < inputLength; i++) {
        output[i] = m_oscillator + input[i];
        ++m_oscillator;
    }
    return inputLength;
  }
};
