//
// Created by murray on 20/8/25.
//

#ifndef CUTESDR_VK6HL_RADIOSETTINGSSOURCE_H
#define CUTESDR_VK6HL_RADIOSETTINGSSOURCE_H

#include "../sink/RadioSettingsSink.h"

class RadioSettingsSource
{
public:
  virtual ~RadioSettingsSource() = default;
  virtual void connect(RadioSettingsSink* pSink) = 0;
  virtual void supply(const RadioSettings& radioSettings) = 0;
};

#endif //CUTESDR_VK6HL_RADIOSETTINGSSOURCE_H