//
// Created by murray on 15/04/23.
//

#ifndef FUNCUBEPLAY_RADIOSETTINGS_H
#define FUNCUBEPLAY_RADIOSETTINGS_H

#include "ReceiverSettings.h"
#include "TransmitterSettings.h"
#include "SettingPath.h"
#include <cstdint>
#include <string>
#include <vector>

#include "SettingsException.h"
#include "../../util/StringUtils.h""


class RadioSettings {
public:
  enum Features
  {
    NONE = 0,
    TX = 0x01,
    RX = 0x02
  };
  ReceiverSettings rxSettings;
  TransmitterSettings txSettings;
  uint32_t changed;

  static SettingPath getSettingPath(const std::string strDottedFeatures)
  {
    std::string featuresLower = StringUtils::toLowerCase(strDottedFeatures);
    std::vector<std::string> featureStrings = StringUtils::split(featuresLower, '.');
    std::vector<uint32_t> features;
    getSettingPath(featureStrings, features);
    return SettingPath(features);
  }

  static void getSettingPath(
    const std::vector<std::string>& featureStrings,
    std::vector<uint32_t>& featuresOut,
    size_t startIndex = 0
    )
  {
    if (startIndex >= featureStrings.size()) {
      throw SettingsException("Invalid setting path");
    }
    if (featureStrings[startIndex] == "rx") {
      featuresOut.push_back(RX);
      if (startIndex + 1 < featureStrings.size()) {
        ReceiverSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
      }
    } else if (featureStrings[startIndex] == "tx") {
      featuresOut.push_back(TX);
      if (startIndex + 1 < featureStrings.size()) {
        TransmitterSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
      }
    } else {
      throw SettingsException("Unknown Radio setting: " + featureStrings[startIndex]);
    }

  }
};

#endif //FUNCUBEPLAY_RADIOSETTINGS_H
