//
// Created by murray on 2025-08-21.
//

#include "GpioLinesRequest.h"
#include "Gpio.h"

#include "GpioException.h"
#include <qdebug.h>

GpioLinesRequest::GpioLinesRequest() :
  m_gpio(Gpio::getInstance())
{
}

void
GpioLinesRequest::initialiseLineStates(const std::vector<GpioLines>& lines)
{
  m_lineStates.clear();

  for (const auto& gpioLine: lines) {
    for (auto lineNo : gpioLine.getLines()) {
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
}
