//
// Created by murray on 17/11/25.
//

#pragma once
#include "IqSourceTypes.h"
#include "core/config-settings/config/audio/IqIoConfig.h"

class IqSourceFactory
{
public:
  static ResultCode create(const Config::IqIo::IqSourceConfigVariant& config, IqSourceVariant& source);


};
