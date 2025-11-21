#pragma once

#include "./Demodulator.h"

class AmDemodulator : public Demodulator
{
public:
  explicit AmDemodulator(uint32_t sampleRate) :
    Demodulator(sampleRate),
    m_z(0.0f)
    {}

  uint32_t processSamples(
      const std::vector<sdrcomplex>& in,
      std::vector<sdrreal>& out,
      uint32_t inputLength
  ) override;

  // uint32_t processSamples(PingPongBuffers<sdrcomplex> buffers, uint32_t inputLength) override;

protected:
  sdrreal m_z;

};
