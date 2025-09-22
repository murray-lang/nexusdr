//
// Created by murray on 2025-08-21.
//

#include "GpioLines.h"
#include "Gpio.h"

#include "GpioException.h"
#include <qdebug.h>

GpioLines::GpioLines() :
  m_gpio(Gpio::getInstance())
{
}

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
