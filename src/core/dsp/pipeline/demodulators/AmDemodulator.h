#pragma once

#include <cstdint>
#include <vector>

#include "./Demodulator.h"

class Mode;

class AmDemodulator : public Demodulator
{
public:
  AmDemodulator(const Mode& mode, uint32_t sampleRate) :
    Demodulator(mode, sampleRate),
    m_zero(0.0f)
    {}

  uint32_t processSamples(
      const vsdrcomplex& in,
      vsdrreal& out,
      uint32_t inputLength
  ) override;

protected:
  sdrreal m_zero;

};
