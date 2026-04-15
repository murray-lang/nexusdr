//
// Created by murray on 1/10/25.
//

#pragma once

#include <vector>

#include "./Demodulator.h"

class SsbDemodulator : public Demodulator
{
public:
  SsbDemodulator(const Mode& mode, uint32_t sampleRate) :
    Demodulator(mode, sampleRate)
  {}

  uint32_t processSamples(
      const ComplexSamplesMax& in,
      RealSamplesMax& out,
      uint32_t inputLength
  ) override;

};
