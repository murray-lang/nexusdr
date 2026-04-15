//
// Created by murray on 17/2/26.
//

#pragma once

#include "../IqPipelineStage.h"

#include <qcoreapplication.h>
#include <cmath>
#include <chrono>
#include <functional>

#include "core/events/EventDispatcher.h"
#include "core/radio/receiver/events/ReceiverMeterEvent.h"

// Pass-through stage that estimates RSSI from complex IQ and posts meter updates.
// Intended placement: after channel filter + decimator, before any AGC.
class SMeterStage : public IqPipelineStage
{
public:
  SMeterStage(QObject* eventTarget,
              std::function<uint32_t()> sampleRateProvider,
              std::function<std::optional<float>()> agcGainDbProvider = nullptr,
              float smoothingTauSeconds = 0.20f,
              float uiUpdateHz = 20.0f) :
    m_eventTarget(eventTarget),
    m_sampleRateProvider(std::move(sampleRateProvider)),
  m_agcGainDbProvider(std::move(agcGainDbProvider)),
    m_tau(std::max(0.01f, smoothingTauSeconds)),
    m_minUpdatePeriod(std::chrono::duration<double>(1.0 / std::max(1.0f, uiUpdateHz)))
  {
  }

  uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) override
  {
    if (m_eventTarget == nullptr || inputLength == 0) {
      return inputLength;
    }
    const ComplexSamplesMax& in = buffers.input();

    const uint32_t fs = m_sampleRateProvider ? m_sampleRateProvider() : 0;
    if (fs == 0) {
      return inputLength;
    }

    // Instantaneous mean power over the block: mean(|x|^2)
    double acc = 0.0;
    for (uint32_t i = 0; i < inputLength; ++i) {
      const auto& x = in[i];
      const auto re = static_cast<double>(x.real());
      const auto im = static_cast<double>(x.imag());
      acc += re * re + im * im;
    }
    const double pInst = acc / static_cast<double>(inputLength);

    // Exponential smoothing with time-constant tau
    const double dt = static_cast<double>(inputLength) / static_cast<double>(fs);
    const double a = 1.0 - std::exp(-dt / static_cast<double>(m_tau));

    if (!m_haveAvg) {
      m_pAvg = pInst;
      m_haveAvg = true;
    } else {
      m_pAvg = (1.0 - a) * m_pAvg + a * pInst;
    }

    // Rate-limit UI events
    const auto now = std::chrono::steady_clock::now();
    if ((now - m_lastPost) >= m_minUpdatePeriod) {
      m_lastPost = now;

      constexpr double eps = 1e-20;
      const double p = std::max(m_pAvg, eps);
      const auto rssiDbFs = static_cast<float>(10.0 * std::log10(p));

      std::optional<float> agcGainDb;
      if (m_agcGainDbProvider) {
        agcGainDb = m_agcGainDbProvider();
      }

      EventDispatcher::postEvent(m_eventTarget, new ReceiverMeterEvent(rssiDbFs, fs, agcGainDb));
    }
    buffers.flip();
    return inputLength;
  }

private:
  QObject* m_eventTarget;
  std::function<uint32_t()> m_sampleRateProvider;
  std::function<std::optional<float>()> m_agcGainDbProvider;

  float m_tau;

  bool m_haveAvg{false};
  double m_pAvg{0.0};

  std::chrono::steady_clock::time_point m_lastPost{};
  std::chrono::duration<double> m_minUpdatePeriod;
};