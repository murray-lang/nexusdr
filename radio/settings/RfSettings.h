//
// Created by murray on 14/04/23.
//

#ifndef FUNCUBEPLAY_RFSETTINGS_H
#define FUNCUBEPLAY_RFSETTINGS_H

#include <cstdint>

struct RfSettings {
  enum Features
  {
    NONE = 0,
    FREQUENCY = 0x01,
    GAIN = 0x02
  };

  uint32_t frequency;
  float gain;
  uint32_t changed;
};
#endif //FUNCUBEPLAY_RFSETTINGS_H
