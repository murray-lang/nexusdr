//
// Created by murray on 1/9/25.
//

#include "FmDemodulator.h"

uint32_t
FmDemodulator::processSamples(
    const ComplexSamplesMax& in,
    RealSamplesMax& out,
    uint32_t inputLength)
{
  for(uint32_t i=0; i<inputLength; i++) {
    sdrcomplex sample = in.at(i);
    float outSample;
    freqdem_demodulate(m_demod, sample, &outSample);
    out.at(i) = outSample;
  }
  return inputLength;
}

void
FmDemodulator::clearState()
{
  if (m_demod) {
    freqdem_destroy(m_demod);
    m_demod = nullptr;
  }
}