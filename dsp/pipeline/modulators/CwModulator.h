//
// Created by murray on 25/11/25.
//

#pragma once
#include "Modulator.h"
#include "dsp/pipeline/oscillators/OscillatorInjector.h"

class CwModulator : public Modulator
{
public:
  CwModulator(const Mode& mode, uint32_t sampleRate) :
  Modulator(mode, sampleRate),
  m_oscillator(sampleRate, mode.getOffset())
  {
  };
  ~CwModulator() override = default;

  void setMode(const Mode& mode) override
  {
    Modulator::setMode(mode);
    m_oscillator.setFrequency(mode.getOffset());
  }

  uint32_t processSamples(const vsdrcomplex& audio, vsdrcomplex& output, uint32_t inputLength) override
  {
    // The input signal provides nothing more than a clock for the CW oscillator
    for (uint32_t i = 0; i < inputLength; i++) {
      output[i] = m_oscillator.getState() * static_cast<sdrreal>(0.05);
      ++m_oscillator;
    }
    return inputLength;
  }

protected:
  Oscillator m_oscillator;
};
