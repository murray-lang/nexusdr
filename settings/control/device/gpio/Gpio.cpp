//
// Created by murray on 2025-08-21.
//

#include "Gpio.h"
#include "GpioLines.h"

#include "GpioException.h"

Gpio::Gpio() :
  m_pChip(nullptr)
{

}

Gpio::~Gpio()
{
  close();
}

bool
Gpio::isPresent(const char *chipPath)
{
  gpiod_chip* pChip = gpiod_chip_open(chipPath);
  if (pChip == nullptr) {
    return false;
  }
  gpiod_chip_close(pChip);
  return true;
}

void
Gpio::open(const char *chipPath) {
  m_pChip = gpiod_chip_open(chipPath);
  if (m_pChip == nullptr) {
    throw GpioException("Failed to open GPIO");
  }
}

void
Gpio::close()
{
  if (m_pChip) {
    gpiod_chip_close(m_pChip);
    m_pChip = nullptr;
  }
}
