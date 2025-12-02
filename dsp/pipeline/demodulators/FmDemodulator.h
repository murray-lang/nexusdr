//
// Created by murray on 1/9/25.
//

#pragma once

#include "./Demodulator.h"

class FmDemodulator : public Demodulator
{
public:
  explicit FmDemodulator(const Mode& mode, uint32_t sampleRate) :
    Demodulator(mode, sampleRate),
    m_prevSample(static_cast<sdrreal>(1.0), static_cast<sdrreal>(0.0)),
    m_maxDeviation(0.1f)
  {}

  uint32_t processSamples(
      const std::vector<sdrcomplex>& in,
      std::vector<sdrreal>& out,
      uint32_t inputLength
  ) override;

  // uint32_t processSamples(PingPongBuffers<sdrcomplex> buffers, uint32_t inputLength) override;
protected:
  sdrreal demodulateSample(const sdrcomplex& sample);

protected:
  sdrcomplex m_prevSample;
  const sdrreal m_maxDeviation;


};
