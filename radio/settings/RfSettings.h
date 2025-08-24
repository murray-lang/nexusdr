//
// Created by murray on 14/04/23.
//

#ifndef FUNCUBEPLAY_RFSETTINGS_H
#define FUNCUBEPLAY_RFSETTINGS_H

#include <cstdint>

#include "SettingsException.h"

class RfSettings {
public:
  enum Features
  {
    NONE = 0,
    FREQUENCY = 0x01,
    GAIN = 0x02
  };

  uint32_t frequency;
  float gain;
  uint32_t changed;

  static void getFeaturePath(
    const std::vector<std::string>& featureStrings,
    std::vector<uint32_t>& features,
    size_t startIndex
    )
  {
    if (startIndex >= featureStrings.size()) {
      throw SettingsException("Invalid setting path");
    }
    if (featureStrings[startIndex] == "frequency") {
      features.push_back(FREQUENCY);
    } else if (featureStrings[startIndex] == "gain") {
      features.push_back(GAIN);
    } else {
      throw SettingsException("Unknown RF setting: " + featureStrings[startIndex]);
    }
  }
};
#endif //FUNCUBEPLAY_RFSETTINGS_H
