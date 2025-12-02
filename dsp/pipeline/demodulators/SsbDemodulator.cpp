//
// Created by murray on 1/10/25.
//

#include "SsbDemodulator.h"

uint32_t
SsbDemodulator::processSamples(
    const std::vector<sdrcomplex>& in,
    std::vector<sdrreal>& out,
    uint32_t inputLength)
{
  if (inputLength == 0) {
    return 0;
  }
  out.resize(inputLength);
  if (m_mode.getType() == Mode::Type::USB) {
    for (uint32_t i = 0; i < inputLength; ++i) {
      out[i] = in[i].real() * 10.0;
    }
  } else {
    for (uint32_t i = 0; i < inputLength; ++i) {
      out[i] = -in[i].real() * 10.0;
    }
  }


  // (OPTIONAL) Remove DC offset, if desired
  sdrreal sum = 0.0;
  for (uint32_t i = 0; i < inputLength; ++i) sum += out[i];
  sdrreal avg = sum / static_cast<sdrreal>(inputLength);
  for (uint32_t i = 0; i < inputLength; ++i) out[i] -= avg;

  return inputLength;
}
