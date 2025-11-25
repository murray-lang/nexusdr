//
// Created by murray on 25/11/25.
//

#include "DigitalOutputLinesRequestImplGpiod.h"
#include "../../digital/DigitalOutput.h"

#include "io/control/device/gpio/GpioException.h"

DigitalOutputLinesRequestImplGpiod::DigitalOutputLinesRequestImplGpiod(gpiod_chip* pChip, const char* consumer):
  m_pChip(pChip),
  m_consumer(consumer),
  m_pLineRequest(nullptr)
{
}

DigitalOutputLinesRequestImplGpiod::~DigitalOutputLinesRequestImplGpiod()
{
  DigitalOutputLinesRequestImplGpiod::release();
}

void
DigitalOutputLinesRequestImplGpiod::request(const char * contextId, const std::vector<DigitalOutput*>& outputs)
{

  gpiod_line_config *lcfg = gpiod_line_config_new();
  if (lcfg == nullptr) {
    throw GpioException("Failed to allocate line config");
  }
  for (const auto& output : outputs) {
    gpiod_line_settings *ls = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(ls, GPIOD_LINE_DIRECTION_OUTPUT);

    const std::vector<uint32_t>& lineNos = output->getLines();
    gpiod_line_config_add_line_settings(lcfg, lineNos.data(), lineNos.size(), ls);
    gpiod_line_settings_free(ls);
  }

  gpiod_request_config *rcfg = gpiod_request_config_new();
  gpiod_request_config_set_consumer(rcfg, contextId);

  gpiod_line_request *pLineRequest = gpiod_chip_request_lines(m_pChip, rcfg, lcfg);
  gpiod_request_config_free(rcfg);
  gpiod_line_config_free(lcfg);
  if (pLineRequest == nullptr) {
    throw GpioException("Failed to request GPIO digital output lines");
  }
  m_pLineRequest = pLineRequest;

}

int
DigitalOutputLinesRequestImplGpiod::setLineValue(uint32_t line, bool value)
{
  return  gpiod_line_request_set_value(m_pLineRequest, line, value ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE);
}

void
DigitalOutputLinesRequestImplGpiod::release()
{
  if (m_pLineRequest) {
    gpiod_line_request_release(m_pLineRequest);
    m_pLineRequest = nullptr;
  }
}

