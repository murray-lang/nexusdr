//
// Created by murray on 13/11/25.
//

#ifndef CUTESDR_VK6HL_MODULATOR_H
#define CUTESDR_VK6HL_MODULATOR_H
#include <cstdint>

#include "SampleTypes.h"

class Modulator
{
public:
  Modulator() = default;
  virtual ~Modulator() = default;

  // virtual uint32_t modulate(const vsdrcomplex audio, vsdrcomplex& output, uint32_t inputLength) = 0;
  virtual uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) = 0;

};

#endif //CUTESDR_VK6HL_MODULATOR_H