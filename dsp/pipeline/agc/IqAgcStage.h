//
// Created by murray on 17/2/26.
//

#pragma once

#include "../IqPipelineStage.h"
#include <liquid/liquid.h>

#include <algorithm>
#include <stdexcept>
#include <complex>

constexpr float SLOW_LOOP_BANDWIDTH = 2e-4f;
constexpr float MEDIUM_LOOP_BANDWIDTH = 7e-4f;
constexpr float FAST_LOOP_BANDWIDTH = 3e-3f;
constexpr float DEFAULT_LOOP_BANDWIDTH = MEDIUM_LOOP_BANDWIDTH;
constexpr float DEFAULT_TARGET_RMS = 0.25f;

class IqAgcStage : public IqPipelineStage
{
public:
  // SSB-friendly defaults: fairly gentle loop, modest target level
  IqAgcStage(float loopBandwidth = DEFAULT_LOOP_BANDWIDTH, float targetRms = DEFAULT_TARGET_RMS) :
    m_agc(nullptr),
    m_targetRms(std::max(1e-6f, targetRms)),
    m_lastGain(1.0f),
    m_off(false)
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



  void setBandwidth(float bandwidth)
  {
    agc_crcf_set_bandwidth(m_agc, bandwidth);
  }

  [[nodiscard]] float getBandwidth() const
  {
    return agc_crcf_get_bandwidth(m_agc);
  }

  void setFast() { setBandwidth(FAST_LOOP_BANDWIDTH); setOff(false); }
  void setMedium() { setBandwidth(MEDIUM_LOOP_BANDWIDTH); setOff(false); }
  void setSlow() { setBandwidth(SLOW_LOOP_BANDWIDTH); setOff(false); }

  void setOff(bool off) { m_off = off; }
  [[nodiscard]] bool isOff() const { return m_off; }

  void setTargetRms(float targetRms)
  {
    agc_crcf_set_signal_level(m_agc, targetRms);
    m_targetRms = targetRms;
  }
  [[nodiscard]] float getTargetRms() const { return m_targetRms; }


  uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) override
  {
    if (m_off) {
      buffers.flip();
      return inputLength;
    }
    const vsdrcomplex& in = buffers.input();
    vsdrcomplex& out = buffers.output();

    // Process sample-by-sample for API safety/portability.
    for (uint32_t i = 0; i < inputLength; ++i) {
      const liquid_float_complex x(in.at(i).real(), in.at(i).imag());

      liquid_float_complex y{};
      agc_crcf_execute(m_agc, x, &y);

      out.at(i) = sdrcomplex(y.real(), y.imag());
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
  bool m_off;
};