//
// Created by murray on 11/11/25.
//

#pragma once
#include "PingPongBufferSink.h"

class IqSink
{
public:
  virtual ~IqSink() = default;
  virtual uint32_t sinkIq(const ComplexSamplesMax& samples, uint32_t length) = 0;
};
