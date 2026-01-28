//
// Created by murray on 27/07/25.
//

#pragma once

#include <config/ControlConfig.h>
#include "ConfigBase.h"
#include "RadioUiConfig.h"
#include "ReceiverConfig.h"
#include "TransmitterConfig.h"

class RadioConfig : public ConfigBase
{
public:
  static constexpr auto type = "radio";

   RadioConfig()
    : ConfigBase(type)
    , m_pReceiver(nullptr)
    , m_pTransmitter(nullptr)
    , m_pControl(nullptr)
    , m_pUiConfig(nullptr)
  {}
  
  ~RadioConfig() override
  {
    delete m_pReceiver;
    delete m_pTransmitter;
    delete m_pControl;
    delete m_pUiConfig;
  }
//   RadioConfig(const RadioConfig& rhs) :
//     m_receiver(rhs.m_receiver),
//     m_control(rhs.m_control)
//   {
// //    operator=(rhs);
//   }
//
//   RadioConfig& operator=(const RadioConfig& rhs)
//   {
//     m_receiver = rhs.m_receiver;
//     m_control = rhs.m_control;
//     return *this;
//   }

  void fromJson(const nlohmann::json& json) override
  {
    if(json.contains("receiver")) {
      m_pReceiver =
        dynamic_cast<ReceiverConfig *>(ConfigFactory::create(ReceiverConfig::type, json[ReceiverConfig::type]));
    }
    if(json.contains("transmitter")) {
     m_pTransmitter =
       dynamic_cast<TransmitterConfig *>(ConfigFactory::create(TransmitterConfig::type, json[TransmitterConfig::type]));
    }
    if (json.contains("control")) {
      m_pControl = dynamic_cast<ControlConfig *>(ConfigFactory::create(ControlConfig::type, json[ControlConfig::type]));
    }
    if (json.contains("ui")) {
      m_pUiConfig = dynamic_cast<RadioUiConfig *>(ConfigFactory::create(RadioUiConfig::type, json["ui"]));
    }
  }

  [[nodiscard]] nlohmann::json toJson() const override
  {
    nlohmann::json receiver;
    if (m_pReceiver) {
      receiver = m_pReceiver->toJson();
    }
    nlohmann::json transmitter;
    if (m_pTransmitter) {
      transmitter = m_pTransmitter->toJson();
    }
    nlohmann::json control;
    if (m_pControl) {
      control = m_pControl->toJson();
    }
    return nlohmann::json{{"receiver", receiver}, {"transmitter", transmitter}, {"control", control}};
  }


  [[nodiscard]] const ReceiverConfig* getReceiver() const { return m_pReceiver; }
  [[nodiscard]] const TransmitterConfig* getTransmitter() const { return m_pTransmitter; }
  // [[nodiscard]] const std::vector<ControllerConfig>& getControllers() const { return m_control; }
  [[nodiscard]] const ControlConfig* getControl() const { return m_pControl; }
  [[nodiscard]] const RadioUiConfig* getUiConfig() const { return m_pUiConfig; }
  const char* getUiFaceName() const
  {
    if (m_pUiConfig != nullptr) {
      return m_pUiConfig->face.c_str();
    }
     return "";
  }

protected:
  ControlConfig* m_pControl;
  ReceiverConfig* m_pReceiver;
  TransmitterConfig* m_pTransmitter;
  RadioUiConfig* m_pUiConfig;
};
