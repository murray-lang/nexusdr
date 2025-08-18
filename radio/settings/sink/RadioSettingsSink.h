//
// Created by murray on 18/08/25.
//

#ifndef RADIOSETTINGSSINK_H
#define RADIOSETTINGSSINK_H
#include "../RadioSettings.h"

class RadioSettingsSink
{
public:
  virtual ~RadioSettingsSink() = default;
  virtual void apply(const RadioSettings& settings) = 0;
};

#endif //RADIOSETTINGSSINK_H
