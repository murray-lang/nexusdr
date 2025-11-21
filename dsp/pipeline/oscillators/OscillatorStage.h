#pragma once

#include "../../../radio/IqPipelineStage.h"
#include "../../blocks/Oscillator.h"
#include <algorithm>

class OscillatorStage : public IqPipelineStage
{
public:
  OscillatorStage() :
    m_oscillator()
  {
  }
  OscillatorStage(uint32_t sampleRate, int32_t frequency) :
    m_oscillator(sampleRate, frequency)
  {
  }
  ~OscillatorStage() override = default;

  OscillatorStage& initialise(uint32_t sampleRate, int32_t frequency) {
    m_oscillator.initialise(sampleRate, frequency);
    return *this;
  }

  OscillatorStage& setFrequency(int32_t frequency) {
      m_oscillator.setFrequency(frequency);
      return *this;
  }

protected:
  Oscillator m_oscillator;
};
