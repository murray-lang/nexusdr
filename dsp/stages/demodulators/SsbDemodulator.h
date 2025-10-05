//
// Created by murray on 1/10/25.
//

#ifndef CUTESDR_VK6HL_SSBDEMODULATOR_H
#define CUTESDR_VK6HL_SSBDEMODULATOR_H
#include "./Demodulator.h"

class SsbDemodulator : public Demodulator
{
public:
  enum Mode { LSB, USB };
  explicit SsbDemodulator(uint32_t sampleRate) :
    Demodulator(sampleRate),
    m_mode(LSB)
  {}

  uint32_t processSamples(
      const std::vector<sdrcomplex>& in,
      std::vector<sdrreal>& out,
      uint32_t inputLength
  ) override;

  void setMode(Mode mode) { m_mode = mode; }
  Mode getMode() { return m_mode; }

  // uint32_t processSamples(PingPongBuffers<sdrcomplex> buffers, uint32_t inputLength) override;

protected:
  Mode m_mode;

};


#endif //CUTESDR_VK6HL_SSBDEMODULATOR_H