//
// Created by murray on 2025-08-21.
//

#include "GpioLines.h"
#include "Gpio.h"

#include "GpioException.h"
#include <qdebug.h>

GpioLines::GpioLines() :
  m_gpio(Gpio::getInstance())
  // m_consumer(consumer)//,
  // m_pLineRequest(nullptr)
{
  // m_pEventBuffer = gpiod_edge_event_buffer_new(64);
}

// GpioLines::~GpioLines()
// {
//   release();
//   gpiod_edge_event_buffer_free(m_pEventBuffer);
// }

// void
// GpioLines::request(
//   GpioCallback* callback,
//   const std::vector<uint32_t>& lines,
//   Direction direction,
//   Bias bias,
//   Edge edge,
//   bool debounce
//   )
// {
//   gpiod_line_config *lcfg = gpiod_line_config_new();
//   if (lcfg == nullptr) {
//     throw GpioException("Failed to allocate line config");
//   }
//   struct gpiod_line_settings *ls = gpiod_line_settings_new();
//   gpiod_line_settings_set_direction(ls, static_cast<gpiod_line_direction>(direction));
//   gpiod_line_settings_set_bias(ls, static_cast<gpiod_line_bias>(bias));
//   gpiod_line_settings_set_edge_detection(ls, static_cast<gpiod_line_edge>(edge));
//   gpiod_line_config_add_line_settings(lcfg, lines.data(), lines.size(), ls);
//   gpiod_line_settings_free(ls);
//
//   gpiod_request_config *rcfg = gpiod_request_config_new();
//   gpiod_request_config_set_consumer(rcfg, m_consumer.c_str());
//
//   m_pLineRequest = gpiod_chip_request_lines(m_gpio.getChip(), rcfg, lcfg);
//   gpiod_request_config_free(rcfg);
//   gpiod_line_config_free(lcfg);
//   if (m_pLineRequest == nullptr) {
//     throw GpioException("Failed to request GPIO lines");
//   }
//   // m_lines = lines;
//   initialiseLineStates(lines);
// }

// void
// GpioLines::release()
// {
//   if (m_pLineRequest) {
//     gpiod_line_request_release(m_pLineRequest);
//     m_pLineRequest = nullptr;
//   }
// }

// int
// GpioLines::getLineValue(uint32_t line)
// {
//   return gpiod_line_request_get_value(m_pLineRequest, line);
// }

// int
// GpioLines::waitEdgeEvents(int64_t timeout_ns) const
// {
//   return gpiod_line_request_wait_edge_events(m_pLineRequest, timeout_ns);
// }
//
// int
// GpioLines::readEdgeEvents(struct gpiod_edge_event_buffer* buf, size_t max_events) const
// {
//   return gpiod_line_request_read_edge_events(m_pLineRequest, buf, max_events);
// }

void
GpioLines::initialiseLineStates(const std::vector<uint32_t>& lines)
{
  m_lineStates.clear();

  for (auto line: lines) {
    int value = getLineValue(line);
    LineState info{
      .line = line,
      .lastRisingTime = 0,
      .lastFallingTime = 0,
      .value = static_cast<uint8_t>(value),
      .changed = false
    };
    m_lineStates[line] = info;
  }
}

// int
// GpioLines::debounce(LineStateMap& changes)
// {
//   constexpr int64_t debounceTimeout= 200'000'000;
//   //std::unordered_map<uint32_t, DebounceInfo> m_debouncedLines;
//   int waitResult = waitEdgeEvents(debounceTimeout);
//
//   if (waitResult < 0) {
//     throw GpioException("Error waiting for edge events");
//   }
//   // Whizz through the event storm until (we think) it's over
//   while(waitResult != 0) {
//     int numEvents = readEdgeEvents(m_pEventBuffer, 64);
//
//     for (int i = 0; i < numEvents; ++i) {
//
//       gpiod_edge_event* ev = gpiod_edge_event_buffer_get_event(m_pEventBuffer, i);
//       uint32_t line = gpiod_edge_event_get_line_offset(ev);
//       gpiod_edge_event_type type = gpiod_edge_event_get_event_type(ev);
//       uint64_t timestamp = gpiod_edge_event_get_timestamp_ns(ev);
//
//       auto stateIter = m_lineStates.find(line);
//       if (stateIter == m_lineStates.end()) {
//         throw GpioException("Received event for unknown line");
//       }
//       LineState& info = stateIter->second;
//       info.changed = true;
//       if (type == GPIOD_EDGE_EVENT_RISING_EDGE) {
//         info.lastRisingTime = timestamp;
//         info.value = 1;
//       } else {
//         info.lastFallingTime = timestamp;
//         info.value = 0;
//       }
//       changes[line] = info;
//     }
//     waitResult = waitEdgeEvents(debounceTimeout);
//     if (waitResult < 0) {
//       throw GpioException("Error waiting for edge events");
//     }
//   }
//   // Now read the affected line values to make sure we have that part of the story straight.
//   for (auto& pair : m_lineStates) {
//     LineState& info = pair.second;
//     if (info.changed) {
//       info.value = static_cast<uint8_t>(getLineValue(pair.first));
//       changes[pair.first] = info;
//       info.changed = false;
//     }
//   }
//   return changes.size();
// }
