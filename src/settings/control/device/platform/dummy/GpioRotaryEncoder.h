//
// Created by murray on 20/8/25.
//

#ifndef DUMMY_GPIOROTARYENCODER_H
#define DUMMY_GPIOROTARYENCODER_H
#include "../../../ControlSink.h"
#include "../../../ControlSource.h"

class GpioRotaryEncoder : public ControlSource
{
public:
  GpioRotaryEncoder();
  ~GpioRotaryEncoder() override = default;

  void initialise(const nlohmann::json& json) override;
  bool discover() override;
  void open() override;
  void close() override;
  void exit() override;
};


#endif //DUMMY_GPIOROTARYENCODER_H