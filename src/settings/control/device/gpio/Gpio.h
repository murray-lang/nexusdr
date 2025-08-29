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

  static Gpio& getInstance() {
    static Gpio instance; // Only created once, thread-safe since C++11
    return instance;
  }

  // Delete copy and move operations
  Gpio(const Gpio&) = delete;
  Gpio& operator=(const Gpio&) = delete;
  Gpio(Gpio&&) = delete;
  Gpio& operator=(Gpio&&) = delete;




  void open(const char *chipPath = defaultChipPath);
  void close();

protected:
  Gpio();
  virtual ~Gpio();
  gpiod_chip * getChip() { return m_pChip; }
protected:
  gpiod_chip *m_pChip;
};


#endif //CUTESDR_VK6HL_GPIO_H