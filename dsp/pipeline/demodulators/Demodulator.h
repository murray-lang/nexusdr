#pragma once

#include "settings/Mode.h"
#include "SampleTypes.h"

class Demodulator
{
public:
  Demodulator(const Mode& mode, uint32_t sampleRate) :
    m_mode(mode),
    m_sampleRate(sampleRate)
  {
  }

  virtual ~Demodulator() = default;

  virtual uint32_t processSamples(
    const vsdrcomplex& in,
    vsdrreal& out,
    uint32_t inputLength
    ) = 0;

  const Mode& getMode() { return m_mode; }
  void setMode(const Mode& mode) { m_mode = mode; }

  virtual void setSampleRate(uint32_t sampleRate) {
    m_sampleRate = sampleRate;
  }
  virtual uint32_t getSampleRate() {
    return m_sampleRate;
  }

  [[nodiscard]] virtual uint32_t getNumOutputChannels() const { return 1; }

protected:
  Mode m_mode;
  uint32_t m_sampleRate;

};
