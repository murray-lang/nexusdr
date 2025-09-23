//
// Created by murray on 18/9/25.
//

#include "GpioLinesImplPiGpio.h"
#include "../../GpioException.h"
#include <qdebug.h>

GpioLinesImplPiGpio::GpioLinesImplPiGpio(const char* consumer) :
  m_pCallback(nullptr),
  m_consumer(consumer)
{
}

GpioLinesImplPiGpio::~GpioLinesImplPiGpio()
{
}

int
GpioLinesImplPiGpio::getLineValue(uint32_t line)
{
    int value = gpioRead(line);
    if (value < 0) {
      throw GpioException("Failed to read GPIO line value");
    }
    return value;
}

void
GpioLinesImplPiGpio::startCallbacks(Callback* callback)
{
  if (m_pCallback != nullptr) {
    throw GpioException("Line state callback already set");
  }
  // std::lock_guard<std::mutex> lock(m_callbackMutex);
  m_pCallback = callback;
}

void
GpioLinesImplPiGpio::stopCallbacks()
{
  m_pCallback = nullptr;
}

void 
GpioLinesImplPiGpio::request(
  const char * contextId,
  const std::vector<uint32_t>& lines,
  GpioLines::Direction direction,
  GpioLines::Bias bias,
  GpioLines::Edge edge
)
{
  for (auto line : lines) {
    setLineDirection(line, direction);
    setLineBias(line, bias);
    gpioNoiseFilter(line, 2000, 2000); // 2ms filter on both steady and active
    
    if (edge != Edge::NONE) {
      // int edgeType = 0;
      // if (edge == Edge::RISING) {
      //   edgeType = RISING_EDGE;
      // } else if (edge == Edge::FALLING) {
      //   edgeType = FALLING_EDGE;
      // } else if (edge == Edge::BOTH) {
      //   edgeType = EITHER_EDGE;
      // }
      int rc = gpioSetAlertFuncEx(line, gpioCallback, this);
      if (rc != 0) {
        throw GpioException("Failed to set GPIO line alert function");
      }
    }
  }
  initialiseLineStates(lines); 
}


void
GpioLinesImplPiGpio::release()
{
  for (const auto& pair : m_lineStates) {
    uint32_t line = pair.first;
    gpioSetAlertFuncEx(line, nullptr, nullptr);
  }
  m_lineStates.clear();
}

void
GpioLinesImplPiGpio::setLineDirection(uint32_t line, Direction direction)
{
    int rc = gpioSetMode(line, direction == Direction::INPUT ? PI_INPUT : PI_OUTPUT);
    if (rc != 0) {
      throw GpioException("Failed to set GPIO line mode");
    }
}

void
GpioLinesImplPiGpio::setLineBias(uint32_t line, Bias bias)
{
    int rc = -1;
    if (bias == Bias::PULL_UP) {
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
GpioLinesImplPiGpio::gpioCallback(int gpio, int level, uint32_t tick, void* userData)
{
  GpioLinesImplPiGpio* self = static_cast<GpioLinesImplPiGpio*>(userData);
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
