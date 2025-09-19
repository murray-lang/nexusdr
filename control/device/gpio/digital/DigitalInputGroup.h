//
// Created by murray on 2025-08-24.
//

#ifndef CUTESDR_VK6HL_DIGITALINPUTGROUP_H
#define CUTESDR_VK6HL_DIGITALINPUTGROUP_H
#include <control/ControlSource.h>
#include "GpioRotaryEncoder.h"
#include "../Gpio.h"
#include "../GpioLines.h"
#include <QThread>
#include <config/DigitalInputConfig.h>
#include "config/DigitalInputGroupConfig.h"

class DigitalInputGroup : public ControlSource, public GpioLines::Callback
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

  void callback(GpioLines::LineStateMap& lineStates) override;

protected:
  void createInputs(const DigitalInputGroupConfig* pConfig);
  void deleteInputs();

  std::vector<uint32_t> gatherLinesFromInputs();
  void readInitialInputStates();

protected:
  std::vector<DigitalInput*> m_inputs;
  std::unique_ptr<GpioLines> m_pLines;
  std::unordered_map<uint32_t, DigitalInput*> m_lineToInputMap;
};


#endif //CUTESDR_VK6HL_DIGITALINPUTGROUP_H