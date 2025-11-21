//
// Created by murray on 18/08/25.
//

#pragma once

// #include "RadioSettings.h"

class RadioSettings;
class SingleSetting;

class RadioSettingsSink
{
public:
  virtual ~RadioSettingsSink() = default;
  virtual void applySettings(const RadioSettings& settings) = 0;
  virtual void applySingleSetting(const SingleSetting& settingDelta) = 0;

};
