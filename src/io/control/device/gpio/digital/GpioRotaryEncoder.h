//
// Created by murray on 20/8/25.
//

#ifndef RPI_GPIOROTARYENCODER_H
#define RPI_GPIOROTARYENCODER_H
#include "../../../ControlSource.h"

#include "DigitalInput.h"
#include "DigitalInputLinesRequest.h"
#include "io/control/ControlException.h"

class GpioRotaryEncoder : public DigitalInput
{
public:
  GpioRotaryEncoder();
  void configure(const DigitalInputConfig* pConfig) override;

  bool handleLineChange(DigitalInputLinesRequest::LineStates& changedLines) override;

protected:
  void notifyMovement(int movement);
  static int calculateMovement(DigitalInputLinesRequest::LineState& a, DigitalInputLinesRequest::LineState& b);
};


#endif //RPI_GPIOROTARYENCODER_H