//
// Created by murray on 27/07/25.
//

#ifndef CUTESDR_VK6HL_RECEIVERCONFIG_H
#define CUTESDR_VK6HL_RECEIVERCONFIG_H
#include "ConfigBase.h"
#include "AudioConfig.h"
#include "ConfigException.h"
#include "ConfigFactory.h"

class ReceiverConfig : public ConfigBase
{
  friend class RadioConfig;
public:
  static constexpr auto type = "receiver";
  explicit ReceiverConfig() : ConfigBase(type), m_pInput(nullptr), m_pOutput(nullptr) {}
  ~ReceiverConfig() override
  {
    delete m_pInput;
    delete m_pOutput;
  };
  // ReceiverConfig(const ReceiverConfig& rhs) :
  //   m_iqInput(rhs.m_iqInput),
  //   m_audioOutput(rhs.m_audioOutput)
  // {
  // }
  //
  // ReceiverConfig& operator=(const ReceiverConfig& rhs)
  // {
  //   m_iqInput = rhs.m_iqInput;
  //   m_audioOutput = rhs.m_audioOutput;
  //   return *this;
  // }

  void initialise(const nlohmann::json& json) override
  {
    if (json.contains("input")) {
      VariantConfig variantConfig(json["input"]);
      m_pInput = ConfigFactory::create(variantConfig);
    } else {
      throw ConfigException("ReceiverConfig: input empty");
    }
    if (json.contains("output")) {
      VariantConfig variantConfig(json["output"]);
      m_pOutput = ConfigFactory::create(variantConfig);
    } else {
      throw ConfigException("ReceiverConfig: output empty");
    }
  }

  [[nodiscard]] const ConfigBase* getInput() const { return m_pInput; }
  [[nodiscard]] const ConfigBase* getOutput() const { return m_pOutput; }

protected:
  ConfigBase* m_pInput;
  ConfigBase* m_pOutput;
};
#endif //CUTESDR_VK6HL_RECEIVERCONFIG_H
