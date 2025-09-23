#ifndef CUTESDR_VK6HL_GPIOIMPLPIGPIO_H
#define CUTESDR_VK6HL_GPIOIMPLPIGPIO_H
#include <iostream>
#include "../../Gpio.h"
#include <pigpio.h>
#include <memory>

class GpioImplPiGpio: public Gpio::Impl
{
public:
  GpioImplPiGpio();
  ~GpioImplPiGpio() override;
    
    static bool isPresent();
    bool open() override;
    bool close() override;

  GpioLines* requestLines(
    const char * contextId,
    const std::vector<uint32_t>& lines,
    GpioLines::Direction direction,
    GpioLines::Bias bias,
    GpioLines::Edge edge
  ) override;

protected:
    int m_initRc;
};


#endif //CUTESDR_VK6HL_GPIOIMPLPIGPIO_H