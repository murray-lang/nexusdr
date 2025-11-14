//
// Created by murray on 15/9/25.
//

#ifndef CUTESDR_VK6HL_DIGITALINPUTGROUPCONFIG_H
#define CUTESDR_VK6HL_DIGITALINPUTGROUPCONFIG_H
#include "ConfigBase.h"
#include "ConfigException.h"
#include "ConfigFactory.h"
#include "DigitalInputConfig.h"
#include "VariantConfig.h"

class DigitalInputGroupConfig : public ConfigBase
{
public:
  static constexpr auto type = "digitalinputgroup";

  DigitalInputGroupConfig() : ConfigBase(type) {}
  ~DigitalInputGroupConfig() override
  {
    for (auto input : m_inputs) {
      delete input;
    }
  }

  void fromJson(const nlohmann::json& json) override
  {
    if (json.contains("inputs")) {
      for (auto& inputConfig : json["inputs"]) {
        VariantConfig variantConfig(inputConfig);
        auto* config = dynamic_cast<DigitalInputConfig*>(ConfigFactory::create(variantConfig));
        m_inputs.push_back(config);
      }
    }
    if (m_inputs.empty()) {
      throw ConfigException("DigitalInputGroupConfig: inputs empty");
    }
  }

  const std::vector<DigitalInputConfig*>& getInputs() const { return m_inputs; }

protected:
  std::vector<DigitalInputConfig*> m_inputs;
};
#endif //CUTESDR_VK6HL_DIGITALINPUTGROUPCONFIG_H