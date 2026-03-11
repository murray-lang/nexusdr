//
// Created by murray on 17/2/26.
//

#pragma once


#include <cstdint>

#include "core/events/EventBase.h"
#include "core/events/EventType.h"

// Simple "latest value" UI event: smoothed RSSI in dBFS (relative to full-scale).
class ReceiverMeterEvent : public EventBase
{
public:
  static const EventType RxMeterEvent;
  ReceiverMeterEvent(float rssiDbFs, uint32_t sampleRate, std::optional<float> agcGainDb = std::nullopt);

  [[nodiscard]] float rssiDbFs() const { return m_rssiDbFs; }
  [[nodiscard]] uint32_t sampleRate() const { return m_sampleRate; }
  [[nodiscard]] const std::optional<float>& agcGainDb() const { return m_agcGainDb; }

private:
  float m_rssiDbFs;
  uint32_t m_sampleRate;
  std::optional<float> m_agcGainDb;
};