//
// Created by murray on 12/01/25.
//

#pragma once

#include "core/dsp/utils/PingPongBuffers.h"

class PingPongBufferSink
{
public:
  virtual ~PingPongBufferSink() = default;
  //  virtual void sink(sdrreal i, sdrreal q) = 0;
  virtual void sink(ComplexPingPongBuffers& buffers, uint32_t length) = 0;
};
