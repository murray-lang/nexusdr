//
// Created by murray on 12/12/25.
//

#pragma once
#include "Oscillator.h"
#include "dsp/pipeline/IqPipelineStage.h"

#define TONE1_FREQUENCY 700
#define TONE2_FREQUENCY 1900

class TwoToneSignalStage : public IqPipelineStage
{
  public:
  TwoToneSignalStage() : m_enabled(false) {}
  TwoToneSignalStage(uint32_t sampleRate) :
    m_enabled(false),
    m_tone1(sampleRate, TONE1_FREQUENCY),
    m_tone2(sampleRate, TONE2_FREQUENCY)
  {
  }
  ~TwoToneSignalStage() override = default;

  void initialise(uint32_t sampleRate)
  {
    m_tone1.initialise(sampleRate, TONE1_FREQUENCY);
    m_tone2.initialise(sampleRate, TONE2_FREQUENCY);
  }

  void setEnabled(bool enabled) { m_enabled = enabled; }
  [[nodiscard]] bool getEnabled() const { return m_enabled; }

  uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) override
  {
    if (!m_enabled) {
      buffers.flip();
      return inputLength;
    }
    return processSamples(buffers.input(), buffers.output(), inputLength);
  }

  uint32_t processSamples(const vsdrcomplex& input, vsdrcomplex& output, uint32_t inputLength)
  {
    for (uint32_t i = 0; i < inputLength; i++) {
      output[i] = m_tone1.getState() + m_tone2.getState();
      ++m_tone1;
      ++m_tone2;
    }
    return inputLength;
  }

protected:
  bool m_enabled;
  Oscillator m_tone1;
  Oscillator m_tone2;
};
