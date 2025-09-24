//
// Created by murray on 20/8/25.
//

#ifndef CUTESDR_VK6HL_RADIOSETTINGSSOURCE_H
#define CUTESDR_VK6HL_RADIOSETTINGSSOURCE_H

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

#endif //CUTESDR_VK6HL_RADIOSETTINGSSOURCE_H