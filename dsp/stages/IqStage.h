#ifndef __IQ_STAGE_H__
#define __IQ_STAGE_H__
#include <cstdint>
#include "../../SampleTypes.h"
#include "../utils/PingPongBuffers.h"

class IqStage
{
public:
  virtual ~IqStage() = default;

  virtual uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength)
    {
      buffers.flip();
      return inputLength;
    }
};

#endif //__IQ_STAGE_H__
