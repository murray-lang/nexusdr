//
// Created by murray on 2025-08-21.
//

#ifndef CUTESDR_VK6HL_GPIO_H
#define CUTESDR_VK6HL_GPIO_H
#include <gpiod.h>
#include <string>
#include <vector>

class GpioLines;

class Gpio {
  friend GpioLines;
public:
  constexpr static auto defaultChipPath = "/dev/gpiochip0";

  Gpio();
  virtual ~Gpio();

  void open(const char *chipPath = defaultChipPath);
  void close();

protected:
  gpiod_chip * getChip() { return m_pChip; }
protected:
  gpiod_chip *m_pChip;
};


#endif //CUTESDR_VK6HL_GPIO_H