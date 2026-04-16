//
// Created by murray on 15/9/25.
//

#pragma once

#include "ConfigBase.h"
#include "ConfigException.h"
#include "ConfigFactory.h"
#include "DigitalInputConfig.h"
#include "VariantConfig.h"

class DigitalInputsConfig : public ConfigBase
{
public:
  static constexpr auto type = "digitalinputs";

  DigitalInputsConfig() : ConfigBase(type) {}
  ~DigitalInputsConfig() override
  {
    for (auto input : m_inputs) {
      delete input;
    }
  }

  void fromJson(JsonVariantConst json) override
  {
    if (json["inputs"]) {
      for (JsonVariantConst inputConfig : json["inputs"].as<JsonArrayConst>()) {
        VariantConfig variantConfig(inputConfig);
        auto* config = dynamic_cast<DigitalInputConfig*>(ConfigFactory::create(variantConfig));
        m_inputs.push_back(config);
      }
    }
    if (m_inputs.empty()) {
      throw ConfigException("DigitalInputsConfig: inputs empty");
    }
  }

  const std::vector<DigitalInputConfig*>& getInputs() const { return m_inputs; }

protected:
  std::vector<DigitalInputConfig*> m_inputs;
};
