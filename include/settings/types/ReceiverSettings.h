//
// Created by murray on 14/04/23.
//

#ifndef FUNCUBEPLAY_RECEIVERSETTINGS_H
#define FUNCUBEPLAY_RECEIVERSETTINGS_H

#include "RfSettings.h"
#include "IfSettings.h"
#include "SettingsException.h"

class ReceiverSettings {
public:
  enum Features
  {
    NONE = 0,
    RF = 0x01,
    IF = 0x02
  };
  RfSettings rfSettings;
  IfSettings ifSettings;
  uint32_t changed;

  static void getFeaturePath(
    const std::vector<std::string>& featureStrings,
    std::vector<uint32_t>& featuresOut,
    size_t startIndex
    )
  {
    if (startIndex >= featureStrings.size()) {
      throw SettingsException("Invalid feature path");
    }
    if (featureStrings[startIndex] == "rf") {
      featuresOut.push_back(RF);
      if (startIndex + 1 < featureStrings.size()) {
        RfSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
      }
    } else if (featureStrings[startIndex] == "if") {
      featuresOut.push_back(IF);
      if (startIndex + 1 < featureStrings.size()) {
        IfSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
      }
    } else {
      throw SettingsException("Unknown receiver setting: " + featureStrings[startIndex]);
    }
  }
};

#endif //FUNCUBEPLAY_RECEIVERSETTINGS_H
