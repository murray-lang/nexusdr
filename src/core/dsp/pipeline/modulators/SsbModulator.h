//
// Created by murray on 13/11/25.
//

#pragma once

#include "Modulator.h"

class SsbModulator : public Modulator
{
public:
  SsbModulator(const Mode& mode, uint32_t sampleRate) :
  Modulator(mode, sampleRate)
  {}
  ~SsbModulator() override = default;

  uint32_t processSamples(const ComplexSamplesMax& audio, ComplexSamplesMax& output, uint32_t inputLength) override
  {
    if (m_mode.getType() == Mode::LSB) {
      for (uint32_t i = 0; i < inputLength; ++i) {
        output.at(i) = sdrcomplex(audio[i].real(), -audio[i].imag()) * m_inputGain;
      }
    } else {
      for (uint32_t i = 0; i < inputLength; ++i) {
        output.at(i) = audio.at(i) * m_inputGain;
      }
    }
    return inputLength;
  }

protected:
  Mode m_mode;
};
