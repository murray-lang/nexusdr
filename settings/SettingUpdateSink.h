//
// Created by murray on 7/1/26.
//

#pragma once
#include "SettingUpdate.h"

class SettingUpdateSink
{
public:
  virtual ~SettingUpdateSink() = default;
  virtual void applySettingUpdate(const SettingUpdate& settingDelta) = 0;
};
