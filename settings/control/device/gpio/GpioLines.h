//
// Created by murray on 2025-08-21.
//

#ifndef CUTESDR_VK6HL_GPIOLINES_H
#define CUTESDR_VK6HL_GPIOLINES_H
#include <string>
#include <cstdint>
#include <unordered_map>
#include "Gpio.h"
#include <gpiod.h>
#include <vector>

class GpioLines {
  friend Gpio;
public:
  enum class Direction {
    AS_IS = 1,
    INPUT,
    OUTPUT
  };

  enum class Bias {
    AS_IS = 1,
    UNKNOWN,
    DISABLED,
    PULL_UP,
    PULL_DOWN
  };

  enum class Edge {
    NONE = 1,
    RISING,
    FALLING,
    BOTH
  };

  struct LineState
  {
    uint32_t line;
    uint64_t lastRisingTime;
    uint64_t lastFallingTime;
    uint8_t value;
    bool changed;
  };
  using LineStateMap = std::unordered_map<uint32_t, LineState>;

  explicit GpioLines(const char* consumer = "");
  ~GpioLines();

  void request(
    const std::vector<uint32_t>& lines,
    Direction direction,
    Bias bias,
    Edge edge
  );

  void release();

  int debounce(LineStateMap& changes);

  int getLineValue(uint32_t line);
  // void setLineValue(uint32_t line, int value);

  // Wait for edge events with a timeout in nanoseconds.
  // Returns:
  //   >0 if events are ready,
  //    0 on timeout,
  //   <0 on error (errno set by libgpiod).
  [[nodiscard]] int waitEdgeEvents(int64_t timeout_ns) const;

  // Read up to max_events edge events into the provided buffer.
  // Returns number of events read, 0 if none available, or <0 on error.
  int readEdgeEvents(struct gpiod_edge_event_buffer* buf, size_t max_events) const;

protected:
  void initialiseLineStates(const std::vector<uint32_t>& lines);

protected:
  Gpio& m_gpio;
  std::vector<uint32_t> m_lines;
  LineStateMap m_lineStates;
  std::string m_consumer;
  gpiod_line_request *m_pLineRequest;
  gpiod_edge_event_buffer* m_pEventBuffer;
};


#endif //CUTESDR_VK6HL_GPIOLINES_H