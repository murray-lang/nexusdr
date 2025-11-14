//
// Created by murray on 8/10/25.
//

#ifndef CUTESDR_VK6HL_RESAMPLER_H
#define CUTESDR_VK6HL_RESAMPLER_H
#include <cstdint>
#include <vector>
#include "SampleTypes.h"
#include <samplerate.h>

class Resampler
{
public:
  Resampler() :
    m_inputSampleRate(0),
    m_outputSampleRate(0),
    m_pSrcStateReal(nullptr),
    m_pSrcStateComplex(nullptr),
    m_ratio(0.0)
  {

  }
  ~Resampler() {
    src_delete(m_pSrcStateReal);
  }

  void configure(uint32_t inputSampleRate, uint32_t outputSampleRate)
  {
    double ratio = static_cast<double>(outputSampleRate) / static_cast<double>(inputSampleRate);
    int error = 0;
    m_pSrcStateReal = src_new(SRC_SINC_BEST_QUALITY, 1, &error);
    if (!m_pSrcStateReal)
      throw std::runtime_error(src_strerror(error));

    m_pSrcStateComplex = src_new(SRC_SINC_BEST_QUALITY, 2, &error);
    if (!m_pSrcStateComplex)
      throw std::runtime_error(src_strerror(error));
    m_ratio = ratio;

  }

  uint32_t processSamples(
    const vsdrreal& in,
    vsdrreal& out,
    uint32_t inputLength
    ) const
  {
    auto max_out = static_cast<long>(inputLength * m_ratio + 32);
    // out.resize(max_out);

    SRC_DATA src_data = {};
    src_data.data_in = in.data();
    src_data.input_frames = inputLength;
    src_data.data_out = out.data();
    src_data.output_frames = max_out;
    src_data.end_of_input = 0;
    src_data.src_ratio = m_ratio;

    int ret = src_process(m_pSrcStateReal, &src_data);
    if (ret != 0)
      throw std::runtime_error(src_strerror(ret));
    return static_cast<uint32_t>(src_data.output_frames_gen);
  }

  uint32_t processSamples(
    ComplexPingPongBuffers& buffers,
    uint32_t inputLength
    ) const
  {
    vsdrcomplex& in = buffers.input();
    vsdrcomplex& out = buffers.output();
    auto max_out = static_cast<long>(inputLength * m_ratio + 32);

    std::vector<float> interleavedIn(inputLength * 2);
    for (size_t i = 0; i < inputLength; ++i) {
      interleavedIn.at(2*i)  = in.at(i).real();
      interleavedIn.at(2*i+1) = in.at(i).imag();
    }
    // out.resize(max_out);
    std::vector<float> interleavedOut(max_out * 2);

    SRC_DATA src_data = {};
    src_data.data_in = interleavedIn.data();
    src_data.input_frames = inputLength;
    src_data.data_out = interleavedOut.data();
    src_data.output_frames = max_out;
    src_data.end_of_input = 0;
    src_data.src_ratio = m_ratio;

    int ret = src_process(m_pSrcStateComplex, &src_data);
    if (ret != 0)
      throw std::runtime_error(src_strerror(ret));

    auto outputLength = static_cast<uint32_t>(src_data.output_frames_gen);
    for (size_t i = 0; i < outputLength; ++i) {
      out.at(i) = sdrcomplex(interleavedOut.at(2*i), interleavedOut.at(2*i+1));
    }
    return outputLength;
  }

protected:
  uint32_t m_inputSampleRate;
  uint32_t m_outputSampleRate;
  SRC_STATE* m_pSrcStateReal;
  SRC_STATE* m_pSrcStateComplex;
  double m_ratio;
  // int m_channels;
};

#endif //CUTESDR_VK6HL_RESAMPLER_H