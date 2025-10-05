//
// Created by murray on 2025-08-21.
//

#include "DigitalInputsRequest.h"
#include "../Gpio.h"

#include "../GpioException.h"
#include <qdebug.h>

#include "DigitalInput.h"

DigitalInputsRequest::DigitalInputsRequest() :
  m_gpio(Gpio::getInstance()),
  m_lineStates(MAX_GPIO)
{
}

void
DigitalInputsRequest::initialiseLineStates(const std::vector<DigitalInput*>& inputs)
{
  // m_lineStates.clear();

  for (const auto input: inputs) {
    for (auto lineNo : input->getLines()) {
      const int value = getLineValue(lineNo);
      const LineState info{
        .line = lineNo,
        .debounce = input->getDebounce(),
        .isDebounced = false,
        // .candidateValue = static_cast<uint8_t>(value),
        // .candidateEdgeTime = 0,
        .firstEdgeTime = 0,
        .lastRisingTime = 0,
        .lastFallingTime = 0,
        .value = static_cast<uint8_t>(value),
        .changed = false,
        .processed = false
      };
      m_lineStates[lineNo] = info;
    }
  }
}
