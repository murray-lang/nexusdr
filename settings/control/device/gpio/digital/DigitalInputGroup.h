//
// Created by murray on 2025-08-24.
//

#ifndef CUTESDR_VK6HL_DIGITALINPUTGROUP_H
#define CUTESDR_VK6HL_DIGITALINPUTGROUP_H
#include <settings/control/ControlSource.h>
#include "GpioRotaryEncoder.h"
#include "../Gpio.h"
#include "../GpioLines.h"
#include <QThread>
#include <config/DigitalInputConfig.h>
#include "config/DigitalInputGroupConfig.h"

class DigitalInputGroup : public ControlSource, public QThread
{
public:
  explicit DigitalInputGroup(const char* consumer = "");
  ~DigitalInputGroup() override;

  // void initialise(const nlohmann::json& json) override;
  void configure(const ConfigBase* pConfig) override;
  bool discover() override;
  void open() override;
  void close() override;
  void exit() override;

  void run() override;

protected:
  void createInputs(const DigitalInputGroupConfig* pConfig);
  void deleteInputs();

  std::vector<uint32_t> gatherLinesFromInputs();
  void readInitialInputStates();

protected:
  bool m_running;
  std::vector<DigitalInput*> m_inputs;
  GpioLines m_lines;
  std::unordered_map<uint32_t, DigitalInput*> m_lineToInputMap;
};


#endif //CUTESDR_VK6HL_DIGITALINPUTGROUP_H