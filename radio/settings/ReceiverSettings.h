//
// Created by murray on 14/04/23.
//

#ifndef FUNCUBEPLAY_RECEIVERSETTINGS_H
#define FUNCUBEPLAY_RECEIVERSETTINGS_H

#include "RfSettings.h"
#include "IfSettings.h"

struct ReceiverSettings {
  enum Changed
  {
    NONE = 0,
    RF = 0x01,
    IF = 0x02
  };
  RfSettings rfSettings;
  IfSettings ifSettings;
  uint32_t changed;
};

#endif //FUNCUBEPLAY_RECEIVERSETTINGS_H
