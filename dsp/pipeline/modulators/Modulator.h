//
// Created by murray on 13/11/25.
//

#pragma once

#include <cstdint>

#include "SampleTypes.h"

class Modulator //: public IqPipelineStage
{
public:
  Modulator() = default;
  virtual ~Modulator() = default;

  virtual uint32_t processSamples(const vsdrcomplex& audio, vsdrcomplex& output, uint32_t inputLength) = 0;
  // virtual uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) = 0;

};
