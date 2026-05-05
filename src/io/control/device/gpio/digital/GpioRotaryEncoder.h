#pragma once

#include "DigitalInput.h"
#include "DigitalInputLinesRequest.h"
#include "core/config-settings/config/control/RotaryEncoderConfig.h"

class GpioRotaryEncoder : public DigitalInput
{
public:
  GpioRotaryEncoder();

  GpioRotaryEncoder(GpioRotaryEncoder&& rhs) = default;
  GpioRotaryEncoder& operator=(GpioRotaryEncoder&& rhs) = default;

  virtual ResultCode configure(const Config::RotaryEncoder::Fields& config);

  bool handleLineChange(DigitalInputLinesRequest::LineStateVector& changedLines) override;

protected:
  void notifyMovement(int movement);
  static int calculateMovement(DigitalInputLinesRequest::LineState& a, DigitalInputLinesRequest::LineState& b);
};
