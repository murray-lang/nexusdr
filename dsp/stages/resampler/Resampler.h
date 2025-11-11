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
    m_pSrcState(nullptr),
    m_ratio(0.0),
    m_channels(1)
  {

  }
  ~Resampler() {
    src_delete(m_pSrcState);
  }

  void configure(uint32_t inputSampleRate, uint32_t outputSampleRate, int channels = 1)
  {
    m_channels = channels;
    double ratio = static_cast<double>(outputSampleRate) / static_cast<double>(inputSampleRate);
    int error = 0;
    m_pSrcState = src_new(SRC_SINC_BEST_QUALITY, m_channels, &error);
    if (!m_pSrcState)
      throw std::runtime_error(src_strerror(error));
    m_ratio = ratio;

  }

  uint32_t processSamples(
    const std::vector<sdrreal>& in,
    std::vector<sdrreal>& out,
    uint32_t inputLength
    )
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

    int ret = src_process(m_pSrcState, &src_data);
    if (ret != 0)
      throw std::runtime_error(src_strerror(ret));
    return static_cast<uint32_t>(src_data.output_frames_gen * m_channels);
  }

protected:
  uint32_t m_inputSampleRate;
  uint32_t m_outputSampleRate;
  SRC_STATE* m_pSrcState;
  double m_ratio;
  int m_channels;
};

#endif //CUTESDR_VK6HL_RESAMPLER_H