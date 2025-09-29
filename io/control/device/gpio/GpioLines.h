//
// Created by murray on 2025-08-21.
//

#ifndef CUTESDR_VK6HL_GPIOLINES_H
#define CUTESDR_VK6HL_GPIOLINES_H
#include <string>
#include <cstdint>
#include <unordered_map>
// #include "Gpio.h"
#include <gpiod.h>
#include <vector>

class Gpio;

class GpioLines {
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

  class Callback
  {
  public:
    virtual ~Callback() = default;
    virtual void callback(LineStateMap& lineStates) = 0;

  };

  GpioLines();
  virtual ~GpioLines() = default;

  virtual void request(
    const char * contextId,
    const std::vector<uint32_t>& lines,
    GpioLines::Direction direction,
    GpioLines::Bias bias,
    GpioLines::Edge edge
  ) = 0;

  virtual void release() = 0;

  virtual void startCallbacks(Callback* callback) = 0;
  virtual void stopCallbacks() = 0;

  virtual int debounce(LineStateMap& changes) = 0;

  virtual int getLineValue(uint32_t line) = 0;

protected:
  void initialiseLineStates(const std::vector<uint32_t>& lines);

  Gpio& m_gpio;
  LineStateMap m_lineStates;
};


#endif //CUTESDR_VK6HL_GPIOLINES_H