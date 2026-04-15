//
// Created by murray on 1/9/25.
//

#pragma once

#include "./Demodulator.h"
#include <liquid/liquid.h>

class FmDemodulator : public Demodulator
{
public:
  explicit FmDemodulator(const Mode& mode, uint32_t sampleRate) :
    Demodulator(mode, sampleRate),
    m_prevSample(static_cast<sdrreal>(1.0), static_cast<sdrreal>(0.0))
  {
    float modulationFactor =
      (static_cast<float>(mode.getHiCut()) - static_cast<float>(mode.getLoCut()))
      / static_cast<float>(sampleRate);

    m_demod = freqdem_create(modulationFactor);
  }

  uint32_t processSamples(
      const ComplexSamplesMax& in,
      RealSamplesMax& out,
      uint32_t inputLength
  ) override;

  // uint32_t processSamples(PingPongBuffers<sdrcomplex> buffers, uint32_t inputLength) override;
protected:
  void clearState();

protected:
  sdrcomplex m_prevSample;
  freqdem m_demod;
};
