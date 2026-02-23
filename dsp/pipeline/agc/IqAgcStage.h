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
  IqAgcStage(AgcSpeed speed = AgcSpeed::DEFAULT) :
    m_agc(nullptr),
    m_lastGain(1.0f),
    m_speed(speed)
  {
    m_agc = agc_crcf_create();
    if (!m_agc) {
      throw std::runtime_error("agc_crcf_create() failed");
    }
    setBandwidth(speed);
  }

  ~IqAgcStage() override
  {
    if (m_agc) {
      agc_crcf_destroy(m_agc);
      m_agc = nullptr;
    }
  }

  void setSpeed(AgcSpeed speed)
  {
    m_speed = speed;
    setBandwidth(speed);
  }
  [[nodiscard]] AgcSpeed getSpeed() const { return m_speed; }

  [[nodiscard]] float getBandwidth() const
  {
    return agc_crcf_get_bandwidth(m_agc);
  }

  bool isOff() const { return m_speed == AgcSpeed::OFF; }

  uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) override
  {
    if (m_speed == AgcSpeed::OFF) {
      buffers.flip();
      return inputLength;
    }
    agc_crcf_execute_block(m_agc, buffers.input().data(), inputLength, buffers.output().data() );
    
    m_lastGain.store(agc_crcf_get_gain(m_agc), std::memory_order_relaxed);
    return inputLength;
  }

  [[nodiscard]] float getGainDb() const
  {
    const float g = std::max(m_lastGain.load(std::memory_order_relaxed), 1e-20f);
    return 20.0f * std::log10(g);
  }

protected:
  float speedToBandwidth(AgcSpeed speed) const
  {
    switch (speed) {
    case AgcSpeed::FAST:
      return FAST_LOOP_BANDWIDTH;
    case AgcSpeed::MEDIUM:
      return MEDIUM_LOOP_BANDWIDTH;
    case AgcSpeed::SLOW:
      return SLOW_LOOP_BANDWIDTH;
    default:
      return DEFAULT_LOOP_BANDWIDTH;
    }
  }

  void setBandwidth(AgcSpeed speed)
  {
    float bw = speedToBandwidth(speed);
    setBandwidth(bw);
  }

  void setBandwidth(float bandwidth)
  {
    agc_crcf_set_bandwidth(m_agc, bandwidth);
  }

private:
  agc_crcf m_agc;
  std::atomic<float> m_lastGain;
  AgcSpeed m_speed;
};