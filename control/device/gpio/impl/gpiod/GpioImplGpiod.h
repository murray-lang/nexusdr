#ifndef CUTESDR_VK6HL_GPIOIMPLGPIOD_H
#define CUTESDR_VK6HL_GPIOIMPLGPIOD_H
#include <iostream>
#include "../../Gpio.h"
#include <gpiod.h>
#include <memory>

class GpioImplGpiod: public Gpio::Impl
{
  constexpr static auto defaultChipPath = "/dev/gpiochip0";
public:
  GpioImplGpiod();
  ~GpioImplGpiod() override;
    
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
    gpiod_chip* m_pChip;
};


#endif //CUTESDR_VK6HL_GPIOIMPLGPIOD_H