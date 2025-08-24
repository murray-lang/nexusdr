//
// Created by murray on 2025-08-24.
//

#include "DigitalInputGroup.h"
#include "../../../config/ConfigException.h"
#include "../../control/ControlSource.h"
#include "../../control/ControlSourceFactory.h"

DigitalInputGroup::DigitalInputGroup(Gpio *pGpio, const char* consumer)
{

}

void
DigitalInputGroup::initialise(const nlohmann::json& json)
{
  if (json.contains("inputs")) {
    for (auto& inputConfig : json["inputs"]) {
      ControlSource* input = ControlSourceFactory::create(inputConfig);
      m_inputs.push_back(input);
    }
  } else {
    throw ConfigException("digitalInputGroup radio control source has no 'inputs' configuration");
  }
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
