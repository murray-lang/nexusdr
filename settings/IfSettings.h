//
// Created by murray on 14/04/23.
//

#ifndef FUNCUBEPLAY_IFSETTINGS_H
#define FUNCUBEPLAY_IFSETTINGS_H

#include <cstdint>

#include "SettingsBase.h"

class IfSettings /*: SettingsBase*/ {
public:
  enum Features
  {
    NONE = 0,
    BANDWIDTH = 0x01,
    GAIN = 0x02
  };

  // void applyDelta(const SettingDelta& delta) override
  // {
  //
  // }

  uint32_t bandwidth;
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
    if (featureStrings[startIndex] == "bandwidth") {
      features.push_back(BANDWIDTH);
    } else if (featureStrings[startIndex] == "gain") {
      features.push_back(GAIN);
    } else {
      throw SettingsException("Unknown IF setting: " + featureStrings[startIndex]);
    }
  }
};
#endif //FUNCUBEPLAY_IFSETTINGS_H
