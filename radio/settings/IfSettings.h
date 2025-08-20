//
// Created by murray on 14/04/23.
//

#ifndef FUNCUBEPLAY_IFSETTINGS_H
#define FUNCUBEPLAY_IFSETTINGS_H

#include <cstdint>

struct IfSettings {
  enum Features
  {
    NONE = 0,
    BANDWIDTH = 0x01,
    GAIN = 0x02
  };
  uint32_t bandwidth;
  float gain;
  uint32_t changed;
};
#endif //FUNCUBEPLAY_IFSETTINGS_H
