//
// Created by murray on 27/07/25.
//

#ifndef CUTESDR_VK6HL_METERINGSTAGE_H
#define CUTESDR_VK6HL_METERINGSTAGE_H

#include "../../../radio/IqPipelineStage.h"
#include "../../../SignalEmitter.h"

class MeteringStage : public IqPipelineStage
{
public:
  typedef enum tagMeteringType {
    eTIMESERIES, eSPECTRUM
  } MeteringType;
public:
  explicit MeteringStage(
      const char* id,
      MeteringType type,
      SignalEmitter& signaller
    ) :
    m_id(id),
    m_type(type),
    m_signaller(signaller)
  {

  }
  uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) override
  {
    switch(m_type)
    {
      case eTIMESERIES:
        break;
      case eSPECTRUM:
        break;
    }
    buffers.flip();
    return inputLength;
  }

protected:
  std::string m_id;
  MeteringType m_type;
  SignalEmitter& m_signaller;
};

#endif //CUTESDR_VK6HL_METERINGSTAGE_H
