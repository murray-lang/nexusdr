//
// Created by murray on 20/8/25.
//

#pragma once


#include "RadioSettingsSink.h"

class RadioSettingsSource
{
public:
  virtual ~RadioSettingsSource() = default;
  virtual void connect(RadioSettingsSink* pSink) = 0;
protected:
  virtual void notifySettings(const RadioSettings& radioSettings) = 0;
  virtual void notifySingleSetting(const SingleSetting& settingDelta) = 0;
};
