//
// Created by murray on 26/11/25.
//

#pragma once
#include "Demodulator.h"

class CwDemodulator : public Demodulator
{
public:  
  CwDemodulator(const Mode& mode, uint32_t sampleRate) :
      Demodulator(mode, sampleRate)
  {}

  uint32_t processSamples(
      const ComplexSamplesMax& in,
      RealSamplesMax& out,
      uint32_t inputLength
  ) override;
};
