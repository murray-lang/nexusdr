//
// Created by murray on 25/11/25.
//

#pragma once
#include "DigitalInputLinesRequestImplGpiod.h"
#include "io/control/device/gpio/digital/DigitalOutputLinesRequest.h"


class DigitalOutputLinesRequestImplGpiod : public DigitalOutputLinesRequest
{
public:
  explicit DigitalOutputLinesRequestImplGpiod(gpiod_chip* pChip, const char* consumer = "");
  ~DigitalOutputLinesRequestImplGpiod() override;

  void request(const char * contextId, const std::vector<DigitalOutput*>& outputs) override;

  void release() override;
  int setLineValue(uint32_t line, bool value) override;

protected:
  gpiod_chip* m_pChip;
  std::string m_consumer;
  gpiod_line_request *m_pLineRequest;
};

