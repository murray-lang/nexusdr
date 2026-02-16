//
// Created by murray on 20/8/25.
//

#pragma once

#include "ControlSource.h"
#include "config-settings/config/ConfigBase.h"


class ControlSourceFactory
{
public:
  static ControlSource* create(const ConfigBase* pConfig);
  static ControlSource* create(const std::string& type);
};

