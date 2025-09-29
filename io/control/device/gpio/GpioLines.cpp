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
GpioLines::initialiseLineStates(const std::vector<GpioLine>& lines)
{
  m_lineStates.clear();

  for (const auto& line: lines) {
    uint32_t lineNo = line.getLineNo();
    const int value = getLineValue(lineNo);
    const LineState info{
      .line = lineNo,
      .lastRisingTime = 0,
      .lastFallingTime = 0,
      .value = static_cast<uint8_t>(value),
      .changed = false
    };
    m_lineStates[lineNo] = info;
  }
}
