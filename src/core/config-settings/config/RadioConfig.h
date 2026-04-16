//
// Created by murray on 27/07/25.
//

#pragma once

#include "core/config-settings/config/ControlConfig.h"
#include "ConfigBase.h"
#include "UiConfig.h"
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

  void fromJson(JsonVariantConst json) override
  {
    if(json[ReceiverConfig::type]) {
      m_pReceiver =
        dynamic_cast<ReceiverConfig *>(ConfigFactory::create(ReceiverConfig::type, json[ReceiverConfig::type]));
    }
    if(json[TransmitterConfig::type]) {
     m_pTransmitter =
       dynamic_cast<TransmitterConfig *>(ConfigFactory::create(TransmitterConfig::type, json[TransmitterConfig::type]));
    }
    if (json[ControlConfig::type]) {
      m_pControl = dynamic_cast<ControlConfig *>(ConfigFactory::create(ControlConfig::type, json[ControlConfig::type]));
    }
    if (json[UiConfig::type]) {
      m_pUiConfig = dynamic_cast<UiConfig *>(ConfigFactory::create(UiConfig::type, json[UiConfig::type]));
    }
  }

  void toJson(JsonObject& json) const override
  {
    if (m_pReceiver) {
      JsonObject receiver = json["receiver"].to<JsonObject>();
      m_pReceiver->toJson(receiver);
    }
    if (m_pTransmitter) {
      JsonObject transmitter = json["transmitter"].to<JsonObject>();
      m_pTransmitter->toJson(transmitter);
    }
    if (m_pControl) {
      JsonObject control = json["control"].to<JsonObject>();
      m_pControl->toJson(control);
    }
  }


  [[nodiscard]] const ReceiverConfig* getReceiver() const { return m_pReceiver; }
  [[nodiscard]] const TransmitterConfig* getTransmitter() const { return m_pTransmitter; }
  // [[nodiscard]] const std::vector<ControllerConfig>& getControllers() const { return m_control; }
  [[nodiscard]] const ControlConfig* getControl() const { return m_pControl; }
  [[nodiscard]] const UiConfig* getUiConfig() const { return m_pUiConfig; }
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
  UiConfig* m_pUiConfig;
};
