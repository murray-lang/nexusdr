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

  void fromJson(JsonVariantConst json) override
  {
    if (json["iqSource"]) {
      VariantConfig variantConfig(json["iqSource"]);
      m_pIqSource = ConfigFactory::create(variantConfig);
    } else {
      throw ConfigException("IqIoConfig: iqSource empty");
    }
    if (json["audioOutput"]) {
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
    // Note: getFields() needs proper serialization implementation
    // TODO: Need to serialize ConfigBase objects to VariantConfig's internal JsonVariant storage
    if (m_pIqSource) {
      f.iqSource.setType(m_pIqSource->getType());
      // TODO: Serialize m_pIqSource to f.iqSource's JsonVariant
    }
    if (m_pAudioOutput) {
      f.audioOutput.setType(m_pAudioOutput->getType());
      // TODO: Serialize m_pAudioOutput to f.audioOutput's JsonVariant
    }
    return f;
  }

  void toJson(JsonObject& json) const override
  {
    if (m_pIqSource) {
      JsonObject input = json["input"].to<JsonObject>();
      input["type"] = m_pIqSource->getType();
      JsonObject config = input["config"].to<JsonObject>();
      m_pIqSource->toJson(config);
    }
    if (m_pAudioOutput) {
      JsonObject output = json["output"].to<JsonObject>();
      output["type"] = m_pAudioOutput->getType();
      JsonObject config = output["config"].to<JsonObject>();
      m_pAudioOutput->toJson(config);
    }
  }


  [[nodiscard]] const ConfigBase* getIqSource() const { return m_pIqSource; }
  [[nodiscard]] const ConfigBase* getAudioOutput() const { return m_pAudioOutput; }

protected:
  ConfigBase* m_pIqSource;
  ConfigBase* m_pAudioOutput;
};