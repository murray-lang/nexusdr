#pragma once

#include <cstdint>
#include "../SampleTypes.h"
#include "../dsp/utils/PingPongBuffers.h"

class IqPipelineStage
{
public:
  virtual ~IqPipelineStage() = default;

  virtual uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) = 0;

};
