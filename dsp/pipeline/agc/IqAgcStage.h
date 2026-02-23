//
// Created by murray on 17/2/26.
//

#pragma once

#include "../IqPipelineStage.h"
#include <liquid/liquid.h>

#include <algorithm>
#include <stdexcept>
#include <complex>

constexpr float SLOW_LOOP_BANDWIDTH = 1e-4f;
constexpr float MEDIUM_LOOP_BANDWIDTH = 1e-3f;
constexpr float FAST_LOOP_BANDWIDTH = 1e-2f;
constexpr float DEFAULT_LOOP_BANDWIDTH = MEDIUM_LOOP_BANDWIDTH;

class IqAgcStage : public IqPipelineStage
{
public:
  // SSB-friendly defaults: fairly gentle loop, modest target level
  IqAgcStage(float loopBandwidth = DEFAULT_LOOP_BANDWIDTH) :
    m_agc(nullptr),
    m_lastGain(1.0f),
    m_off(false)
  {
    m_agc = agc_crcf_create();
    if (!m_agc) {
      throw std::runtime_error("agc_crcf_create() failed");
    }
    agc_crcf_set_bandwidth(m_agc, loopBandwidth);
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

  uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) override
  {
    if (m_off) {
      buffers.flip();
      return inputLength;
    }
    const vsdrcomplex& in = buffers.input();
    vsdrcomplex& out = buffers.output();

    agc_crcf_execute_block(m_agc, (liquid_float_complex *)in.data(), inputLength, (liquid_float_complex *)out.data() );
    
    m_lastGain.store(agc_crcf_get_gain(m_agc), std::memory_order_relaxed);
    return inputLength;
  }

  [[nodiscard]] float getGainDb() const
  {
    const float g = std::max(m_lastGain.load(std::memory_order_relaxed), 1e-20f);
    return 20.0f * std::log10(g);
  }

private:
  agc_crcf m_agc;
  std::atomic<float> m_lastGain;
  bool m_off;
};