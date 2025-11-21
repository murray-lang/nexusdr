#pragma once

#include "../../../radio/IqPipelineStage.h"

class Demodulator
{
public:
  virtual ~Demodulator() = default;

  explicit Demodulator(uint32_t sampleRate) :
    m_sampleRate(sampleRate)
  {
  }

    virtual uint32_t processSamples(
    const std::vector<sdrcomplex>& in,
    std::vector<sdrreal>& out,
    uint32_t inputLength
    ) = 0;

  // virtual uint32_t processSamples(PingPongBuffers<sdrcomplex> buffers, uint32_t inputLength) = 0;

  virtual void setOutputRate(uint32_t sampleRate) {
    m_sampleRate = sampleRate;
  }
  virtual uint32_t getOutputRate() {
    return m_sampleRate;
  }

protected:
  uint32_t m_sampleRate;

};
