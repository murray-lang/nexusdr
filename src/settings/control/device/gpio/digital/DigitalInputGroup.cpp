//
// Created by murray on 2025-08-24.
//

#include "DigitalInputGroup.h"
#include "../../../include/config/ConfigException.h"
#include <settings/control/ControlSource.h>
#include <settings/control/ControlSourceFactory.h>

DigitalInputGroup::DigitalInputGroup(const char* consumer) :
  m_lines(consumer)
{

}

DigitalInputGroup::~DigitalInputGroup()
{
  deleteAllInputs();

}

void
DigitalInputGroup::initialise(const nlohmann::json& json)
{
  createAllInputs(json);

}

bool
DigitalInputGroup::discover()
{
    return false;
}

void
DigitalInputGroup::open()
{

}

void
DigitalInputGroup::close()
{

}

void
DigitalInputGroup::exit()
{

}

void
DigitalInputGroup::createAllInputs(const nlohmann::json& json)
{
  deleteAllInputs();
  if (json.contains("inputs")) {
    for (auto& inputConfig : json["inputs"]) {
      std::string strType = inputConfig["type"];
      ControlSource* input = ControlSourceFactory::create(strType);
      m_inputs.push_back(input);
    }
  } else {
    throw ConfigException("digitalInputGroup radio control source has no 'inputs' configuration");
  }
}

void
DigitalInputGroup::deleteAllInputs()
{
  for (auto input : m_inputs) {
    delete input;
  }
  m_inputs.clear();
}
