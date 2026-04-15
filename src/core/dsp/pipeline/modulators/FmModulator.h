//
// Created by murray on 18/12/25.
//

#pragma once
#include <liquid/liquid.h>

#include "Modulator.h"

class FmModulator : public Modulator
{
public:
  explicit FmModulator(const Mode& mode, uint32_t sampleRate) : Modulator(mode, sampleRate)
  {
    float modulationFactor =
      (static_cast<float>(mode.getHiCut()) - static_cast<float>(mode.getLoCut()))
      / static_cast<float>(sampleRate);
    m_mod = freqmod_create(modulationFactor);
  }
  ~FmModulator() override
  {
    clearState();
  }

  uint32_t processSamples(
      const ComplexSamplesMax& in,
      ComplexSamplesMax& out,
      uint32_t inputLength
  ) override
  {
    for (uint32_t i = 0; i < inputLength; i++) {
      liquid_float_complex sampleOut;
      // freqmod only needs a real value, so we take the hypotenuse of the analytic signal
      freqmod_modulate(m_mod, std::abs(in.at(i)), &sampleOut);
      out.at(i) = sdrcomplex(sampleOut.real(), sampleOut.imag());
    }
    return inputLength;
  }

protected:
  void clearState()
  {
    if (m_mod) {
      freqmod_destroy(m_mod);
      m_mod = nullptr;
    }
  }

protected:
  freqmod m_mod;
};