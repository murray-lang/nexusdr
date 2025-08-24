//
// Created by murray on 2025-08-21.
//

#ifndef CUTESDR_VK6HL_GPIOLINES_H
#define CUTESDR_VK6HL_GPIOLINES_H
#include <string>

#include "Gpio.h"


class GpioLines {
  friend Gpio;
public:
  GpioLines(Gpio *pGpio, const char* consumer = "");
  ~GpioLines();

  void request(
    const std::vector<uint32_t>& lines,
    gpiod_line_direction direction,
    gpiod_line_bias bias,
    gpiod_line_edge edge
  );

  void release();

protected:
  Gpio *m_pGpio;
  std::vector<uint32_t> m_lines;
  std::string m_consumer;
  gpiod_line_request *m_pLineRequest;

};


#endif //CUTESDR_VK6HL_GPIOLINES_H