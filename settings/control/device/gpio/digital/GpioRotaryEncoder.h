//
// Created by murray on 20/8/25.
//

#ifndef RPI_GPIOROTARYENCODER_H
#define RPI_GPIOROTARYENCODER_H
#include <settings/control/ControlSource.h>

#include "DigitalInput.h"
#include "../GpioLines.h"

class GpioRotaryEncoder : public DigitalInput
{
public:
  explicit GpioRotaryEncoder(GpioLines& lines) :
    DigitalInput(lines)
  {};
  void initialise(const nlohmann::json& json) override;

  bool handleLineChange(GpioLines::LineStateMap& changedLines) override;

protected:
  static int calculateMovement(GpioLines::LineState& a, GpioLines::LineState& b);
};


#endif //RPI_GPIOROTARYENCODER_H