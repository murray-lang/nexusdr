//
// Created by murray on 20/11/25.
//

#pragma once
#include "ConfigBase.h"
#include "ConfigException.h"
#include "ConfigFactory.h"
#include "VariantConfig.h"

// Plain struct listing configuration members for programmer visibility
struct IqIoConfigFields {
  VariantConfig iqSource;
  VariantConfig audioOutput;
};

class IqIoConfig : public ConfigBase, public IqIoConfigFields
{
public:
  static constexpr auto type = "iqIo";
  explicit IqIoConfig() : ConfigBase(type), m_pIqSource(nullptr), m_pAudioOutput(nullptr) {}
  ~IqIoConfig() override
  {
    delete m_pIqSource;
    delete m_pAudioOutput;
  };

  void fromJson(const nlohmann::json& json) override
  {
    if (json.contains("iqSource")) {
      VariantConfig variantConfig(json["iqSource"]);
      m_pIqSource = ConfigFactory::create(variantConfig);
    } else {
      throw ConfigException("IqIoConfig: iqSource empty");
    }
    if (json.contains("audioOutput")) {
      VariantConfig variantConfig(json["audioOutput"]);
      m_pAudioOutput = ConfigFactory::create(variantConfig);
    } else {
      throw ConfigException("IqIoConfig: audioOutput empty");
    }
  }

  // Convenience helpers to work with the plain struct form
  void setFields(const IqIoConfigFields& f)
  {
    static_cast<IqIoConfigFields&>(*this) = f;
    delete m_pIqSource; m_pIqSource = nullptr;
    delete m_pAudioOutput; m_pAudioOutput = nullptr;
    if (!f.iqSource.getType().empty()) m_pIqSource = ConfigFactory::create(f.iqSource);
    if (!f.audioOutput.getType().empty()) m_pAudioOutput = ConfigFactory::create(f.audioOutput);
  }

  [[nodiscard]] IqIoConfigFields getFields() const
  {
    IqIoConfigFields f;
    if (m_pIqSource) f.iqSource = nlohmann::json{{"type", m_pIqSource->getType()}, {"config", m_pIqSource->toJson()}};
    if (m_pAudioOutput) f.audioOutput = nlohmann::json{{"type", m_pAudioOutput->getType()}, {"config", m_pAudioOutput->toJson()}};
    return f;
  }

  [[nodiscard]] nlohmann::json toJson() const override
  {
    nlohmann::json input;
    if (m_pIqSource) {
      input = nlohmann::json{{"type", m_pIqSource->getType()}, {"config", m_pIqSource->toJson()}};
    }
    nlohmann::json output;
    if (m_pAudioOutput) {
      output = nlohmann::json{{"type", m_pAudioOutput->getType()}, {"config", m_pAudioOutput->toJson()}};
    }
    return nlohmann::json{{"input", input}, {"output", output}};
  }


  [[nodiscard]] const ConfigBase* getIqSource() const { return m_pIqSource; }
  [[nodiscard]] const ConfigBase* getAudioOutput() const { return m_pAudioOutput; }

protected:
  ConfigBase* m_pIqSource;
  ConfigBase* m_pAudioOutput;
};