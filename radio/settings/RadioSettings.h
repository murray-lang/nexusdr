//
// Created by murray on 15/04/23.
//

#ifndef FUNCUBEPLAY_RADIOSETTINGS_H
#define FUNCUBEPLAY_RADIOSETTINGS_H

#include "ReceiverSettings.h"
#include "TransmitterSettings.h"
#include <vector>


struct RadioSettings {
  enum Changed
  {
    NONE = 0,
    TX = 0x01,
    RX = 0x02
  };
  ReceiverSettings rxSettings;
  TransmitterSettings txSettings;
  uint32_t changed;
};

#endif //FUNCUBEPLAY_RADIOSETTINGS_H
