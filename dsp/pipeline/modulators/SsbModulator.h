//
// Created by murray on 13/11/25.
//

#pragma once

#include "Modulator.h"

class SsbModulator : public Modulator
{
public:
  enum Mode { LSB, USB };
  explicit SsbModulator(Mode mode = LSB) : m_mode(mode) {}

  uint32_t processSamples(const vsdrcomplex& audio, vsdrcomplex& output, uint32_t inputLength) override
  {
    // vsdrcomplex& input = buffers.input();
    // vsdrcomplex& output = buffers.output();
    if (m_mode == LSB) {
      for (uint32_t i = 0; i < inputLength; ++i) {
        output.at(i) = sdrcomplex(audio[i].real(), -audio[i].imag());
      }
    } else {
      for (uint32_t i = 0; i < inputLength; ++i) {
        output.at(i) = audio.at(i);
      }
    }
    return inputLength;
  }

  void setMode(Mode mode) { m_mode = mode; }
  [[nodiscard]] Mode getMode() const { return m_mode; }


protected:
  Mode m_mode;
};
