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
  sdrreal phaseDiff = std::arg(sample * std::conj(m_prevSample));
  m_prevSample = sample;
  return phaseDiff;

}