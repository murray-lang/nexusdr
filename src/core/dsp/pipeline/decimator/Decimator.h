//
// Created by murray on 16/07/25.
//

#pragma once

#include <cstdint>
#include "../IqPipelineStage.h"
#include "../../utils/PingPongBuffers.h"
#include <liquid/liquid.h>


class Decimator : public IqPipelineStage
{
public:
  Decimator() :
    m_inputSampleRate(192000),
    m_outputSampleRate(48000),
    m_pResampleState(nullptr)
  {}

  Decimator(uint32_t inputSampleRate, uint32_t outputSampleRate) :
    m_inputSampleRate(inputSampleRate),
    m_outputSampleRate(outputSampleRate),
    m_pResampleState(nullptr)
  {
    configure(inputSampleRate, outputSampleRate);
  }

  ~Decimator() override {
    clearState();
  }

  uint32_t configure(uint32_t inputSampleRate, uint32_t outputSampleRate)
  {
    m_inputSampleRate = inputSampleRate;
    m_outputSampleRate = outputSampleRate;

    clearState();

    //m_decimationFactor = inputSampleRate/outputSampleRate;
    float ratio = static_cast<float>(outputSampleRate) / static_cast<float>(inputSampleRate);

    // Design parameters
    unsigned int m = 12;    // Filter delay
    float bw = 0.45f;       // Bandwidth
    float as = 60.0f;       // Stop-band attenuation (dB)
    unsigned int npb = 32;  // Number of polyphase banks

    m_pResampleState = resamp_crcf_create(ratio, m, bw, as, npb);
    return outputSampleRate;
  }

  [[nodiscard]] uint32_t getOutputSampleRate() const { return m_outputSampleRate; }

  uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) override
  {
    ComplexSamplesMax& input = buffers.input();
    ComplexSamplesMax& output = buffers.output();
    uint32_t numWritten = 0;

    for (uint32_t i = 0; i < inputLength; ++i)
    {
      unsigned int n;
      resamp_crcf_execute(m_pResampleState, input.at(i), &output.at(numWritten), &n);
      numWritten += n;
    }

    return numWritten;
  }

protected:
  void clearState()
  {
    if (m_pResampleState) {
      resamp_crcf_destroy(m_pResampleState);
      m_pResampleState = nullptr;
    }
  }

protected:
  uint32_t m_inputSampleRate;
  uint32_t m_outputSampleRate;
  resamp_crcf m_pResampleState;
};

