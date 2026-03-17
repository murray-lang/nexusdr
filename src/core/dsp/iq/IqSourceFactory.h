//
// Created by murray on 17/11/25.
//

#pragma once
#include <string>

#include "IqSource.h"
#include "core/config-settings/config/ConfigBase.h"


class IqSourceFactory
{
public:
  static IqSource* create(const ConfigBase* pConfig);
  static IqSource* create(const std::string& type);


};
