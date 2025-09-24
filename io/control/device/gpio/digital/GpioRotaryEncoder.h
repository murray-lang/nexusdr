//
// Created by murray on 20/8/25.
//

#ifndef RPI_GPIOROTARYENCODER_H
#define RPI_GPIOROTARYENCODER_H
#include "../../../ControlSource.h"

#include "DigitalInput.h"
#include "../GpioLines.h"
#include "io/control/ControlException.h"

class GpioRotaryEncoder : public DigitalInput
{
public:
  explicit GpioRotaryEncoder() = default;
  void configure(const DigitalInputConfig* pConfig) override;

  bool handleLineChange(GpioLines::LineStateMap& changedLines) override;

protected:
  void notifySettings(const RadioSettings& settings) override
  {
    throw ControlException("GpioRotaryEncoder cannot notify settings, only a single setting.");
  }
  void notifyMovement(int movement);
  static int calculateMovement(GpioLines::LineState& a, GpioLines::LineState& b);
};


#endif //RPI_GPIOROTARYENCODER_H