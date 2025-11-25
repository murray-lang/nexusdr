//
// Created by murray on 12/01/25.
//

#pragma once

#include "../../SampleTypes.h"
#include "dsp/utils/PingPongBuffers.h"

template<typename T>
class PingPongBufferSink
{
public:
  virtual ~PingPongBufferSink() = default;
  //  virtual void sink(sdrreal i, sdrreal q) = 0;
  virtual void sink(PingPongBuffers<T>& buffers, uint32_t length) = 0;
};
