//
// Created by murray on 2025-08-24.
//

#include "DigitalInputGroup.h"
#include <config/ConfigException.h>
#include "../../../ControlSource.h"
#include "../../../ControlSourceFactory.h"
#include <poll.h>
#include <qdebug.h>
#include <config/DigitalInputConfig.h>

#include "DigitalInputFactory.h"
#include "config/DigitalInputGroupConfig.h"
#include "control/device/gpio/GpioException.h"

DigitalInputGroup::DigitalInputGroup(const char* consumer)
  // m_lines(consumer),
{

}

DigitalInputGroup::~DigitalInputGroup()
{
  deleteInputs();

}

void
DigitalInputGroup::configure(const ConfigBase* pConfig)
{
  const auto* config = dynamic_cast<const DigitalInputGroupConfig*>(pConfig);
  createInputs(config);
}

bool
DigitalInputGroup::discover()
{
    return Gpio::isPresent();
}

void
DigitalInputGroup::open()
{
  if (m_pLines != nullptr) {
    throw GpioException("DigitalInputGroup already open");
  }
  std::vector<uint32_t> lines = gatherLinesFromInputs();
  Gpio& gpio = Gpio::getInstance();
  GpioLines* pLines = gpio.requestLines("digitalInputs", lines, GpioLines::Direction::INPUT, GpioLines::Bias::PULL_UP, GpioLines::Edge::BOTH);
  m_pLines.reset(pLines);
  m_pLines->startCallbacks(this);
}

void
DigitalInputGroup::close()
{
  if (m_pLines == nullptr) {
    throw GpioException("DigitalInputGroup not open");
  }
  m_pLines->stopCallbacks();
  m_pLines->release();
  m_pLines.reset();
}

void
DigitalInputGroup::exit()
{

}

void
DigitalInputGroup::createInputs(const DigitalInputGroupConfig* pConfig)
{
  deleteInputs();
  for (const auto& pInputConfig : pConfig->getInputs()) {
    DigitalInput* input = DigitalInputFactory::create(pInputConfig);
    if (input == nullptr) {
      throw ConfigException("digitalInputGroup input has unknown input type: " + pConfig->getType());
    }
    m_inputs.push_back(input);
  }
}

void
DigitalInputGroup::deleteInputs()
{
  for (auto input : m_inputs) {
    delete input;
  }
  m_inputs.clear();
}

std::vector<uint32_t>
DigitalInputGroup::gatherLinesFromInputs()
{
  m_lineToInputMap.clear();

  std::vector<uint32_t> result;
  for (auto input : m_inputs) {
    const std::vector<uint32_t>& lines = input->getLines();
    result.insert(result.end(), lines.begin(), lines.end());
    for (auto line : lines) {
      m_lineToInputMap[line] = input;
    }
  }
  return result;
}

void
DigitalInputGroup::readInitialInputStates()
{
  // for (auto encoder : m_encoders) {
  //   encoder->initialiseState(m_lines);
  // }
}

void
DigitalInputGroup::callback(GpioLines::LineStateMap& lineStates)
{
  for (auto& input : m_inputs) {
    input->handleLineChange(lineStates);
  }
}

