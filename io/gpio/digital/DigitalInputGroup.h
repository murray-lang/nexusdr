//
// Created by murray on 2025-08-24.
//

#ifndef CUTESDR_VK6HL_DIGITALINPUTGROUP_H
#define CUTESDR_VK6HL_DIGITALINPUTGROUP_H
#include "../../control/ControlSource.h"
#include "../Gpio.h"
#include "../GpioLines.h"

class DigitalInputGroup : public ControlSource
{
public:
  DigitalInputGroup(Gpio *pGpio, const char* consumer = "");
  ~DigitalInputGroup() override = default;

  void initialise(const nlohmann::json& json) override;
  bool discover() override;
  void open() override;
  void close() override;
  void exit() override;

protected:
  std::vector<ControlSource*> m_inputs;
  GpioLines* m_pLines;
};


#endif //CUTESDR_VK6HL_DIGITALINPUTGROUP_H