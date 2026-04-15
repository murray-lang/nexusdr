//
// Created by murray on 12/12/25.
//

#pragma once
#include "Oscillator.h"
#include "core/dsp/pipeline/IqPipelineStage.h"

#define TONE1_FREQUENCY -700
#define TONE2_FREQUENCY -1900
#define TONE3_FREQUENCY 3000


class TwoToneSignalStage : public IqPipelineStage
{
  public:
  TwoToneSignalStage() : m_enabled(false), m_gain(0.1) {}
  TwoToneSignalStage(uint32_t sampleRate) :
    m_enabled(false),
    m_gain(0.1),
    m_tone1(sampleRate, TONE1_FREQUENCY),
    m_tone2(sampleRate, TONE2_FREQUENCY)//,
    // m_tone3(sampleRate, TONE3_FREQUENCY)
  {
  }
  ~TwoToneSignalStage() override = default;

  void initialise(uint32_t sampleRate)
  {
    m_tone1.initialise(sampleRate, TONE1_FREQUENCY);
    m_tone2.initialise(sampleRate, TONE2_FREQUENCY);
    // m_tone3.initialise(sampleRate, TONE3_FREQUENCY);
  }

  void setEnabled(bool enabled) { m_enabled = enabled; }
  [[nodiscard]] bool getEnabled() const { return m_enabled; }
  void setGain(sdrreal gain) { m_gain = gain; }
  [[nodiscard]] sdrreal getGain() const { return m_gain; } 

  uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) override
  {
    if (!m_enabled) {
      buffers.flip();
      return inputLength;
    }
    return processSamples(buffers.input(), buffers.output(), inputLength);
  }

  uint32_t processSamples(const ComplexSamplesMax& input, ComplexSamplesMax& output, uint32_t inputLength)
  {
    for (uint32_t i = 0; i < inputLength; i++) {
      output.at(i) = (m_tone1.getState() + m_tone2.getState() /*+ m_tone3.getState()*/) * m_gain;
      ++m_tone1;
      ++m_tone2;
      // ++m_tone3;
    }
    return inputLength;
  }

protected:
  bool m_enabled;
  sdrreal m_gain;
  Oscillator m_tone1;
  Oscillator m_tone2;
  // Oscillator m_tone3;
};
