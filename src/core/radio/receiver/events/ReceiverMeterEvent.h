#pragma once
#include "../IqReceiverMetering.h"

#include <cstdint>

#include "core/events/EventBase.h"
#include "core/events/EventType.h"

// Simple "latest value" UI event: smoothed RSSI in dBFS (relative to full-scale).
class ReceiverMeterEvent : public EventBase
{
public:
  static const EventType RxMeterEvent;
  ReceiverMeterEvent(float rssiDbFs, uint32_t sampleRate, std::optional<float> agcGainDb = std::nullopt);
  ReceiverMeterEvent(const IqReceiverMetering& metering);

  [[nodiscard]] float rssiDbFs() const { return m_metering.rssiDbFs; }
  [[nodiscard]] uint32_t sampleRate() const { return m_metering.sampleRate; }
  [[nodiscard]] const std::optional<float>& agcGainDb() const { return m_metering.agcGainDb; }
  [[nodiscard]] const IqReceiverMetering& metering() const { return m_metering; }

private:
  IqReceiverMetering m_metering;
};