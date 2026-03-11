//
// Created by murray on 13/11/25.
//

#pragma once

#include <cstdint>

#include "../../../SampleTypes.h"

#include "core/config-settings/settings/Mode.h"

class Modulator //: public IqPipelineStage
{
public:
  Modulator(const Mode& mode, uint32_t sampleRate) : m_mode(mode), m_sampleRate(sampleRate), m_inputGain(1.0) {}
  virtual ~Modulator() = default;

  virtual uint32_t processSamples(const vsdrcomplex& audio, vsdrcomplex& output, uint32_t inputLength) = 0;

  virtual void setSampleRate(uint32_t sampleRate) {
    m_sampleRate = sampleRate;
  }
  virtual uint32_t getSampleRate() {
    return m_sampleRate;
  }

  const Mode& getMode() { return m_mode; }
  virtual void setMode(const Mode& mode) { m_mode = mode; }

  void setInputGain(sdrreal gain) { m_inputGain = gain; }
  [[nodiscard]] sdrreal getInputGain() const { return m_inputGain; }

protected:
  Mode m_mode;
  uint32_t m_sampleRate;
  sdrreal m_inputGain;

};
