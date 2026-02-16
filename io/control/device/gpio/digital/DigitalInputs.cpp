//
// Created by murray on 2025-08-24.
//

#include "DigitalInputs.h"
#include "config-settings/config/ConfigException.h"
#include "config-settings/config/DigitalInputsConfig.h"
#include "../../../ControlSource.h"
#include "../../../ControlSourceFactory.h"
#include <poll.h>
#include <qdebug.h>

#include "DigitalInputFactory.h"
#include "io/control/device/gpio/GpioException.h"
#include "io/control/device/gpio/GpioLines.h"

DigitalInputs::DigitalInputs(const char* consumer) :
  m_internalSink(this)
{

}

DigitalInputs::~DigitalInputs()
{
  deleteInputs();

}

void
DigitalInputs::configure(const ConfigBase* pConfig)
{
  const auto* config = dynamic_cast<const DigitalInputsConfig*>(pConfig);
  createInputs(config);
}

bool
DigitalInputs::discover()
{
    return Gpio::isPresent();
}

void
DigitalInputs::open()
{
  if (m_pLines != nullptr) {
    throw GpioException("DigitalInputs already open");
  }
  createLineToInputMap();
  Gpio& gpio = Gpio::getInstance();
  DigitalInputLinesRequest* pLines = gpio.requestDigitalInputs("digitalInputs", m_inputs);
  m_pLines.reset(pLines);
  m_pLines->startCallbacks(this);
}

void
DigitalInputs::close()
{
  if (m_pLines == nullptr) {
    throw GpioException("DigitalInputs not open");
  }
  m_pLines->stopCallbacks();
  m_pLines->release();
  m_pLines.reset();
}

void
DigitalInputs::exit()
{

}

void
DigitalInputs::createInputs(const DigitalInputsConfig* pConfig)
{
  deleteInputs();
  for (const auto& pInputConfig : pConfig->getInputs()) {
    DigitalInput* input = DigitalInputFactory::create(pInputConfig);
    if (input == nullptr) {
      throw ConfigException("digitalInputs input has unknown input type: " + pConfig->getType());
    }
    input->connectSettingUpdateSink(&m_internalSink);
    m_inputs.push_back(input);
  }
}

void
DigitalInputs::deleteInputs()
{
  for (auto input : m_inputs) {
    delete input;
  }
  m_inputs.clear();
}

void
DigitalInputs::createLineToInputMap()
{
  m_lineToInputMap.clear();
  for (const auto input : m_inputs) {
    const std::vector<uint32_t>& lineNos = input->getLines();
    for (const auto& lineNo : lineNos) {
      m_lineToInputMap[lineNo] = input;
    }
  }
}

void
DigitalInputs::readInitialInputStates()
{
  // for (auto encoder : m_encoders) {
  //   encoder->initialiseState(m_lines);
  // }
}

void
DigitalInputs::callback(DigitalInputLinesRequest::LineStates& lineStates)
{
  for (auto& input : m_inputs) {
    input->handleLineChange(lineStates);
  }
}
