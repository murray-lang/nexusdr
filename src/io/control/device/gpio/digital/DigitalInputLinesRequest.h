//
// Created by murray on 2025-08-21.
//

#pragma once

#include <string>
#include <cstdint>
#include <unordered_map>
// #include "Gpio.h"
#include <gpiod.h>
#include <vector>

#include "../GpioLines.h"
// #include "digital/DigitalInput.h"

class Gpio;
class DigitalInput;

class DigitalInputLinesRequest
{
public:

  struct LineState
  {
    uint32_t line;
    bool debounce;
    bool isDebounced;
    // uint8_t candidateValue;
    // uint64_t candidateEdgeTime;
    uint64_t firstEdgeTime;
    uint64_t lastRisingTime;
    uint64_t lastFallingTime;
    uint8_t value;
    bool changed;
    bool processed;
  };
  using LineStateMap = std::unordered_map<uint32_t, LineState>;
  using LineStates = std::vector<LineState>;

  class Callback
  {
  public:
    virtual ~Callback() = default;
    virtual void callback(LineStates& lineStates) = 0;

  };

  DigitalInputLinesRequest();
  virtual ~DigitalInputLinesRequest() = default;

  virtual void request(const char * contextId, const std::vector<DigitalInput*>& inputs) = 0;

  virtual void release() = 0;

  virtual void startCallbacks(Callback* callback) = 0;
  virtual void stopCallbacks() = 0;

  // virtual int debounce(LineStateMap& changes) = 0;

  virtual int getLineValue(uint32_t line) = 0;

protected:
  void initialiseLineStates(const std::vector<DigitalInput*>& inputs);

  Gpio& m_gpio;
  // LineStateMap m_lineStates;
  LineStates m_lineStates;
};
