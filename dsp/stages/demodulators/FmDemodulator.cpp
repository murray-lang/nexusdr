//
// Created by murray on 1/9/25.
//

#include "FmDemodulator.h"

DemodulatorSettings defaultFmSettings =
{
  .label = "FM",
  .hiCutMin = 500,
  .hiCutMax = 10000,
  .lowCutMin = -500,
  .lowCutMax = -10000,
  .defaultFreqClickResolution = 1000,
  .filterClickResolution = 100,
  .symmetrical = true,
};

uint32_t
FmDemodulator::processSamples(
    const std::vector<sdrcomplex>& in,
    std::vector<sdrreal>& out,
    uint32_t inputLength)
{
  for(uint32_t i=0; i<inputLength; i++) {
    sdrcomplex sample = in.at(i);
    out.at(i) = demodulateSample(sample);
  }
  return inputLength;
}

sdrreal
FmDemodulator::demodulateSample(const sdrcomplex& sample) {
  // Quadrature FM discriminator:
  // output = (I_prev * Q_now - Q_prev * I_now) / (I_now^2 + Q_now^2)
  sdrreal i1 = std::real(m_prevSample);
  sdrreal q1 = std::imag(m_prevSample);
  sdrreal i2 = std::real(sample);
  sdrreal q2 = std::imag(sample);

  sdrreal denominator = i2 * i2 + q2 * q2;
  if (denominator == 0.0f) denominator = 1e-12f; // Avoid divide by zero

  sdrreal out = (i1 * q2 - q1 * i2) / denominator;

  m_prevSample = sample;
  return out;

}