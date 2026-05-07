//
// Created by murray on 27/07/25.
//

#pragma once

#include "../audio/IqIoConfig.h"

namespace Config::Receiver
{
  static constexpr auto type = "receiver";

  struct Fields
  {
    IqIo::Fields iqIo;
  };

  extern ResultCode fromJson(JsonVariantConst json, Fields& fields);
}

// using ReceiverConfig = Config::Receiver::Fields;
