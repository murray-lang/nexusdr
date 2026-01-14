//
// Created by murray on 20/8/25.
//

#pragma once


#include "RadioSettingsSink.h"
#include "SettingUpdateSource.h"

class RadioSettingsSource : public SettingUpdateSource
{
public:
  ~RadioSettingsSource() override = default;
  virtual void connect(RadioSettingsSink* pSink) = 0;
protected:
  virtual void notifySettings(const RadioSettings& radioSettings) = 0;
};
