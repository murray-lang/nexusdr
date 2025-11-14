//
// Created by murray on 27/07/25.
//

#ifndef CUTESDR_VK6HL_TRANSMITTERCONFIG_H
#define CUTESDR_VK6HL_TRANSMITTERCONFIG_H
#include "ConfigBase.h"
#include "ConfigException.h"
#include "ConfigFactory.h"
#include "VariantConfig.h"

// Plain struct listing configuration members for programmer visibility
struct TransmitterConfigFields {
  VariantConfig input;
  VariantConfig output;
};

class TransmitterConfig : public ConfigBase, public TransmitterConfigFields
{
  friend class RadioConfig;
public:
  static constexpr auto type = "transmitter";
  explicit TransmitterConfig() : ConfigBase(type), m_pInput(nullptr), m_pOutput(nullptr) {}
  ~TransmitterConfig() override
  {
    delete m_pInput;
    delete m_pOutput;
  };

  // TransmitterConfig(const TransmitterConfig& rhs)
  // {
  //   operator=(rhs);
  // }
  //
  // TransmitterConfig& operator=(const TransmitterConfig& rhs)
  // {
  //   return *this;
  // }

  void fromJson(const nlohmann::json& json) override
  {
    if (json.contains("input")) {
      VariantConfig variantConfig(json["input"]);
      m_pInput = ConfigFactory::create(variantConfig);
    } else {
      throw ConfigException("TransmitterConfig: input empty");
    }
    if (json.contains("output")) {
      VariantConfig variantConfig(json["output"]);
      m_pOutput = ConfigFactory::create(variantConfig);
    } else {
      throw ConfigException("TransmitterConfig: output empty");
    }
  }

  // Convenience helpers to work with the plain struct form
  void setFields(const TransmitterConfigFields& f)
  {
    static_cast<TransmitterConfigFields&>(*this) = f;
    delete m_pInput; m_pInput = nullptr;
    delete m_pOutput; m_pOutput = nullptr;
    if (!f.input.getType().empty()) m_pInput = ConfigFactory::create(f.input);
    if (!f.output.getType().empty()) m_pOutput = ConfigFactory::create(f.output);
  }

  [[nodiscard]] TransmitterConfigFields getFields() const
  {
    TransmitterConfigFields f;
    if (m_pInput) f.input = nlohmann::json{{"type", m_pInput->getType()}, {"config", m_pInput->toJson()}};
    if (m_pOutput) f.output = nlohmann::json{{"type", m_pOutput->getType()}, {"config", m_pOutput->toJson()}};
    return f;
  }

  [[nodiscard]] nlohmann::json toJson() const override
  {
    nlohmann::json input;
    if (m_pInput) {
      input = nlohmann::json{{"type", m_pInput->getType()}, {"config", m_pInput->toJson()}};
    }
    nlohmann::json output;
    if (m_pOutput) {
      output = nlohmann::json{{"type", m_pOutput->getType()}, {"config", m_pOutput->toJson()}};
    }
    return nlohmann::json{{"input", input}, {"output", output}};
  }


  [[nodiscard]] const ConfigBase* getInput() const { return m_pInput; }
  [[nodiscard]] const ConfigBase* getOutput() const { return m_pOutput; }

protected:
  ConfigBase* m_pInput;
  ConfigBase* m_pOutput;
};
#endif //CUTESDR_VK6HL_TRANSMITTERCONFIG_H
