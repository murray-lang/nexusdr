//
// Created by murray on 6/08/25.
//

#pragma once

#include <string>

#include "ControlSink.h"
#include "../../config-settings/config/ConfigBase.h"


class ControlSinkFactory
{
public:
  static ControlSink* create(const ConfigBase* pConfig);
  static ControlSink* create(const std::string& type);
  // static std::string toLowerCase(const std::string& str);
};
