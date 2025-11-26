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
      const std::vector<sdrcomplex>& in,
      std::vector<sdrreal>& out,
      uint32_t inputLength
  ) override;
};
