//
// Created by murray on 18/9/25.
//

#include "DigitalInputsRequestImplPiGpio.h"
#include "../../GpioException.h"
#include <qdebug.h>

DigitalInputsRequestImplPiGpio::DigitalInputsRequestImplPiGpio(const char* consumer) :
  m_pCallback(nullptr),
  m_consumer(consumer)
{
}

DigitalInputsRequestImplPiGpio::~DigitalInputsRequestImplPiGpio()
{
}

int
DigitalInputsRequestImplPiGpio::getLineValue(uint32_t line)
{
    int value = gpioRead(line);
    if (value < 0) {
      throw GpioException("Failed to read GPIO line value");
    }
    return value;
}

void
DigitalInputsRequestImplPiGpio::startCallbacks(Callback* callback)
{
  if (m_pCallback != nullptr) {
    throw GpioException("Line state callback already set");
  }
  // std::lock_guard<std::mutex> lock(m_callbackMutex);
  m_pCallback = callback;
}

void
DigitalInputsRequestImplPiGpio::stopCallbacks()
{
  m_pCallback = nullptr;
}

void 
DigitalInputsRequestImplPiGpio::request(const char * contextId, const std::vector<DigitalInput*>& lines)
{
  for (auto gpioLines : lines) {
    for (auto lineNo : gpioLines.getLines()) {
      setLineDirection(lineNo, gpioLines.getDirection());
      setLineBias(lineNo, gpioLines.getBias());
      gpioNoiseFilter(lineNo, 2000, 2000); // 2ms filter on both steady and active

      if (edge != Edge::NONE) {
        // int edgeType = 0;
        // if (edge == Edge::RISING) {
        //   edgeType = RISING_EDGE;
        // } else if (edge == Edge::FALLING) {
        //   edgeType = FALLING_EDGE;
        // } else if (edge == Edge::BOTH) {
        //   edgeType = EITHER_EDGE;
        // }
        int rc = gpioSetAlertFuncEx(lineNo, gpioCallback, this);
        if (rc != 0) {
          throw GpioException("Failed to set GPIO line alert function");
        }
      }
    }
    }

  initialiseLineStates(lines); 
}


void
DigitalInputsRequestImplPiGpio::release()
{
  for (const auto& pair : m_lineStates) {
    uint32_t line = pair.first;
    gpioSetAlertFuncEx(line, nullptr, nullptr);
  }
  m_lineStates.clear();
}

void
DigitalInputsRequestImplPiGpio::setLineDirection(uint32_t line, GpioLines::Direction direction)
{
    int rc = gpioSetMode(line, direction == GpioLines::Direction::INPUT ? PI_INPUT : PI_OUTPUT);
    if (rc != 0) {
      throw GpioException("Failed to set GPIO line mode");
    }
}

void
DigitalInputsRequestImplPiGpio::setLineBias(uint32_t line, GpioLines::Bias bias)
{
    int rc = -1;
    if (bias == GpioLines::Bias::PULL_UP) {
      rc = gpioSetPullUpDown(line, PI_PUD_UP);
    } else if (bias == Bias::PULL_DOWN) {
      rc =  gpioSetPullUpDown(line, PI_PUD_DOWN);
    } else if (bias == Bias::DISABLED) {
      rc =  gpioSetPullUpDown(line, PI_PUD_OFF);
    } else {
      throw GpioException("Unsupported bias setting");
    }
    if (rc != 0) {
      throw GpioException("Failed to set GPIO line bias");
    }
}

void
DigitalInputsRequestImplPiGpio::gpioCallback(int gpio, int level, uint32_t tick, void* userData)
{
  DigitalInputsRequestImplPiGpio* self = static_cast<DigitalInputsRequestImplPiGpio*>(userData);
  if (self->m_pCallback) {
    LineStateMap changes;
    auto stateIter = self->m_lineStates.find(gpio);
    if (stateIter != self->m_lineStates.end()) {
      LineState& info = stateIter->second;
      info.changed = true;
      uint64_t timestamp = static_cast<uint64_t>(tick) * 1000; // microseconds to nanoseconds
      if (level == 1) {
        info.lastRisingTime = timestamp;
        info.value = 1;
      } else if (level == 0) {
        info.lastFallingTime = timestamp;
        info.value = 0;
      }
      changes[gpio] = info;
      self->m_pCallback->callback(changes);
    }
  }
}
