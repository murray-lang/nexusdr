//
// Created by murray on 18/08/25.
//

#ifndef RADIOSETTINGSSINK_H
#define RADIOSETTINGSSINK_H
#include "RadioSettings.h"

class RadioSettingsSink
{
public:
  virtual ~RadioSettingsSink() = default;
  virtual void applySettings(const RadioSettings& settings) = 0;
  virtual void applySingleSetting(const SettingDelta& settingDelta) = 0;

};

#endif //RADIOSETTINGSSINK_H
