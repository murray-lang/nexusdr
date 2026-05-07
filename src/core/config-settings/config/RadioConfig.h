#pragma once
#include "CrossPlatformTypes.h"
#include "control/ControlConfig.h"
#include "ui/UiConfig.h"
#include "receiver/ReceiverConfig.h"
#include "transmitter/TransmitterConfig.h"

namespace Config::Radio
{
  static constexpr auto tag = "radio";

  struct Fields
  {
    Control::Fields control;
    optional<Receiver::Fields> receiver;
    optional<Transmitter::Fields> transmitter;
    optional<Ui::Fields> ui;
  };

  extern ResultCode fromJson(JsonVariantConst json, Fields& fields);
}

// extern Config::Radio::Fields tempConfig;

// using RadioConfig = Config::Radio::Fields;