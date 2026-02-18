//
// Created by murray on 17/2/26.
//

#pragma once

#include "../IqPipelineStage.h"
#include <liquid/liquid.h>

#include <algorithm>
#include <stdexcept>

// Complex IQ AGC stage using liquid-dsp agc_crcf.
// Intended placement: after SMeterStage, before demodulator.
class IqAgcStage : public IqPipelineStage
{
public:
  // SSB-friendly defaults: fairly gentle loop, modest target level
  IqAgcStage(float loopBandwidth = 7e-4f, float targetRms = 0.25f) :
    m_agc(nullptr),
    m_targetRms(std::max(1e-6f, targetRms)),
    m_lastGain(1.0f)
  {
    m_agc = agc_crcf_create();
    if (!m_agc) {
      throw std::runtime_error("agc_crcf_create() failed");
    }
    agc_crcf_set_bandwidth(m_agc, loopBandwidth);

    // liquid-dsp AGC uses a "desired signal level" (gain drives output toward this).
    // For complex float, this is effectively an RMS-ish target.
    agc_crcf_set_signal_level(m_agc, m_targetRms);
  }

  ~IqAgcStage() override
  {
    if (m_agc) {
      agc_crcf_destroy(m_agc);
      m_agc = nullptr;
    }
  }

  uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) override
  {
    const vsdrcomplex& in = buffers.input();
    vsdrcomplex& out = buffers.output();

    // Process sample-by-sample for API safety/portability.
    for (uint32_t i = 0; i < inputLength; ++i) {
      liquid_float_complex y{};
      agc_crcf_execute(m_agc,
                      static_cast<liquid_float_complex>(in[i]),
                      &y);
      out[i] = static_cast<vsdrcomplex::value_type>(y);
      m_lastGain.store(agc_crcf_get_gain(m_agc), std::memory_order_relaxed);
    }
    return inputLength;
  }

  [[nodiscard]] float getGainDb() const
  {
    const float g = std::max(m_lastGain.load(std::memory_order_relaxed), 1e-20f);
    return 20.0f * std::log10(g);
  }

private:
  agc_crcf m_agc;
  float m_targetRms;
  std::atomic<float> m_lastGain;
};