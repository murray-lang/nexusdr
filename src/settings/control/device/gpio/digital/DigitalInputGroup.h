//
// Created by murray on 2025-08-24.
//

#ifndef CUTESDR_VK6HL_DIGITALINPUTGROUP_H
#define CUTESDR_VK6HL_DIGITALINPUTGROUP_H
#include <settings/control/ControlSource.h>
#include "../Gpio.h"
#include "../GpioLines.h"

class DigitalInputGroup : public ControlSource
{
public:
  explicit DigitalInputGroup(const char* consumer = "");
  ~DigitalInputGroup() override;

  void initialise(const nlohmann::json& json) override;
  bool discover() override;
  void open() override;
  void close() override;
  void exit() override;

protected:
  void createAllInputs(const nlohmann::json& json);
  void deleteAllInputs();

protected:
  std::vector<ControlSource*> m_inputs;
  GpioLines m_lines;
};


#endif //CUTESDR_VK6HL_DIGITALINPUTGROUP_H