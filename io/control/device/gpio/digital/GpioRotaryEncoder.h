//
// Created by murray on 20/8/25.
//

#ifndef RPI_GPIOROTARYENCODER_H
#define RPI_GPIOROTARYENCODER_H
#include "../../../ControlSource.h"

#include "DigitalInput.h"
#include "DigitalInputsRequest.h"
#include "io/control/ControlException.h"

class GpioRotaryEncoder : public DigitalInput
{
public:
  GpioRotaryEncoder();
  void configure(const DigitalInputConfig* pConfig) override;

  bool handleLineChange(DigitalInputsRequest::LineStates& changedLines) override;

protected:
  void notifySettings(const RadioSettings& settings) override
  {
    throw ControlException("GpioRotaryEncoder cannot notify settings, only a single setting.");
  }
  void notifyMovement(int movement);
  static int calculateMovement(DigitalInputsRequest::LineState& a, DigitalInputsRequest::LineState& b);
};


#endif //RPI_GPIOROTARYENCODER_H