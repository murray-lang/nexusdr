#pragma once

#include "base/ConfigBase.h"
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
    Receiver::Fields receiver;
    Transmitter::Fields transmitter;
    Ui::Fields ui;
  };

  extern Result fromJson(JsonVariantConst json, Fields& fields);
}

extern Config::Radio::Fields tempConfig;

using RadioConfig = Config::Radio::Fields;