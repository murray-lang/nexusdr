#pragma once

#include "../../Gpio.h"
#include "CrossPlatformTypes.h"
#include <gpiod.h>

#include "ResultCode.h"


class DigitalOutputLinesRequest;
class DigitalInputLinesRequest;
class DigitalInputVariantVector;
class DigitalOutputVariantVector;

class Gpio {

  constexpr static auto defaultChipPath = "/dev/gpiochip0";
public:
  Gpio();
  virtual ~Gpio();

  static Gpio& getInstance() {
    static Gpio instance; // Only created once, thread-safe since C++11
    return instance;
  }

  // Delete copy and move operations
  Gpio(const Gpio&) = delete;
  Gpio& operator=(const Gpio&) = delete;
  Gpio(Gpio&&) = delete;
  Gpio& operator=(Gpio&&) = delete;

  static bool isPresent();
  ResultCode open();
  void close();

  ResultCode requestDigitalInputs(
    const char * contextId,
    const DigitalInputVariantVector& inputs,
    DigitalInputLinesRequest& dilr
  );
  ResultCode requestDigitalOutputs(
    const char * contextId,
    const DigitalOutputVariantVector& outputs,
    DigitalOutputLinesRequest& dolr
  );

protected:
  gpiod_chip* m_pChip;
};
