//
// Created by murray on 8/10/25.
//
#pragma once

#include <cstdint>
#include <vector>
#include "../../../SampleTypes.h"
#include <liquid/liquid.h>

class Resampler: public IqPipelineStage
{
public:
  Resampler() :
    m_inputSampleRate(0),
    m_outputSampleRate(0),
    m_pResampleStateReal(nullptr),
    m_pResampleStateComplex(nullptr),
    m_ratio(0.0)
  {

  }
  ~Resampler() override {
    clearState();
  }

  void configure(uint32_t inputSampleRate, uint32_t outputSampleRate)
  {
    m_inputSampleRate = inputSampleRate;
    m_outputSampleRate = outputSampleRate;
    initialise();
  }

  void initialise()
  {
    clearState();
    float ratio = static_cast<float>(m_outputSampleRate) / static_cast<float>(m_inputSampleRate);

    // Liquid-dsp resampler parameters
    unsigned int m = 12;      // filter semi-length (delay)
    float bw = 0.45f;         // resampling bandwidth
    float as = 100.0f;         // stop-band attenuation [dB]

    m_pResampleStateReal = resamp_rrrf_create(ratio, m, bw, as, 32);
    m_pResampleStateComplex = resamp_crcf_create(ratio, m, bw, as, 32);

    m_ratio = ratio;
  }

  uint32_t processSamples(
    const vsdrreal& in,
    vsdrreal& out,
    uint32_t inputLength
    ) const
  {
    unsigned int numWritten = 0;
    for (uint32_t i = 0; i < inputLength; ++i) {
      unsigned int n;
      resamp_rrrf_execute(m_pResampleStateReal, in.at(i), &out.at(numWritten), &n);
      numWritten += n;
    }
    return numWritten;
  }

  uint32_t processSamples(
    ComplexPingPongBuffers& buffers,
    uint32_t inputLength
    ) override
  {
    vsdrcomplex& in = buffers.input();
    vsdrcomplex& out = buffers.output();
    unsigned int numWritten = 0;
    for (uint32_t i = 0; i < inputLength; ++i) {
      unsigned int n;
      // liquid-dsp uses float complex which is usually binary compatible with std::complex<float>
      resamp_crcf_execute(m_pResampleStateComplex, in.at(i), &out.at(numWritten), &n);
      numWritten += n;
    }

    return numWritten;
  }

protected:
  void clearState()
  {
    if (m_pResampleStateReal) {
      resamp_rrrf_destroy(m_pResampleStateReal);
      m_pResampleStateReal = nullptr;
    }
    if (m_pResampleStateComplex) {
      resamp_crcf_destroy(m_pResampleStateComplex);
      m_pResampleStateComplex = nullptr;
    }
  }

protected:
  uint32_t m_inputSampleRate;
  uint32_t m_outputSampleRate;
  resamp_rrrf m_pResampleStateReal;
  resamp_crcf m_pResampleStateComplex;
  double m_ratio;
};
