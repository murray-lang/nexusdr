//
// Created by murray on 2025-08-21.
//

#include "GpioLines.h"

#include "GpioException.h"

GpioLines::GpioLines(Gpio *pGpio, const char* consumer ) :
  m_pGpio(pGpio),
  m_lines(),
  m_consumer(consumer),
  m_pLineRequest(nullptr)
{

}

GpioLines::~GpioLines()
{
  release();
}

void
GpioLines::request(
  const std::vector<uint32_t>& lines,
  gpiod_line_direction direction,
  gpiod_line_bias bias,
  gpiod_line_edge edge
  )
{
  m_lines = lines;
  gpiod_line_config *lcfg = gpiod_line_config_new();
  if (lcfg == nullptr) {
    throw GpioException("Failed to allocate line config");
  }
  struct gpiod_line_settings *ls = gpiod_line_settings_new();
  gpiod_line_settings_set_direction(ls, direction);
  gpiod_line_settings_set_bias(ls, bias);
  gpiod_line_settings_set_edge_detection(ls, edge);
  gpiod_line_config_add_line_settings(lcfg, m_lines.data(), m_lines.size(), ls);
  gpiod_line_settings_free(ls);

  gpiod_request_config *rcfg = gpiod_request_config_new();
  gpiod_request_config_set_consumer(rcfg, m_consumer.c_str());

  m_pLineRequest = gpiod_chip_request_lines(m_pGpio->getChip(), rcfg, lcfg);
  gpiod_request_config_free(rcfg);
  gpiod_line_config_free(lcfg);
  if (m_pLineRequest == nullptr) {
    throw GpioException("Failed to request GPIO lines");
  }
}

void
GpioLines::release()
{
  if (m_pLineRequest) {
    gpiod_line_request_release(m_pLineRequest);
    m_pLineRequest = nullptr;
  }
}