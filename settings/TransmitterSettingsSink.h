//
// Created by murray on 11/11/25.
//

#ifndef CUTESDR_VK6HL_TRANSMITTERSETTINGSSINK_H
#define CUTESDR_VK6HL_TRANSMITTERSETTINGSSINK_H
#include "TransmitterSettings.h"
class TransmitterSettingsSink
{
public:
  virtual ~TransmitterSettingsSink() = default;
  virtual void apply(const TransmitterSettings& txSettings) = 0;
};
#endif //CUTESDR_VK6HL_TRANSMITTERSETTINGSSINK_H