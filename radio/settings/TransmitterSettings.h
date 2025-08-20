//
// Created by murray on 14/04/23.
//

#ifndef FUNCUBEPLAY_TRANSMITTERSETTINGS_H
#define FUNCUBEPLAY_TRANSMITTERSETTINGS_H

#include "RfSettings.h"

struct TransmitterSettings {
  enum Features
  {
    NONE = 0,
    RF = 0x01
  };
  RfSettings rf;
  uint32_t changed;
};

#endif //FUNCUBEPLAY_TRANSMITTERSETTINGS_H
