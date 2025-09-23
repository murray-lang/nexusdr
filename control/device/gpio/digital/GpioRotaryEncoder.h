//
// Created by murray on 20/8/25.
//

#ifndef RPI_GPIOROTARYENCODER_H
#define RPI_GPIOROTARYENCODER_H
#include "../../../ControlSource.h"

#include "DigitalInput.h"
#include "../GpioLines.h"

class GpioRotaryEncoder : public DigitalInput
{
public:
  explicit GpioRotaryEncoder() = default;
  void configure(const DigitalInputConfig* pConfig) override;

  bool handleLineChange(GpioLines::LineStateMap& changedLines) override;

protected:
  static int calculateMovement(GpioLines::LineState& a, GpioLines::LineState& b);
};


#endif //RPI_GPIOROTARYENCODER_H