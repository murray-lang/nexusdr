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
#include "../util/StringUtils.h"


class RadioSettings : public SettingsBase {
public:
  enum Features
  {
    NONE = 0,
    TX = 0x01,
    RX = 0x02
  };

  RadioSettings() = default;
  RadioSettings(const RadioSettings& rhs) = default;
  
  ~RadioSettings() override = default;

  RadioSettings& operator=(const RadioSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      rxSettings = rhs.rxSettings;
      txSettings = rhs.txSettings;
    }
    return *this;
  }

  bool applySetting(const SingleSetting& setting, int startIndex) override
  {
    if (startIndex >= setting.getPath().getFeatures().size()) {
      throw SettingsException("Invalid setting path");
    }
    bool settingChange = false;
    uint32_t feature = setting.getPath().getFeatures()[startIndex];
    if (feature == TX) {
      settingChange = txSettings.applySetting(setting, startIndex + 1);
    } else if (feature == RX) {
      settingChange = rxSettings.applySetting(setting, startIndex + 1);
    }
    if (settingChange) {
      changed |= feature;
    }
    return settingChange;
  }

  void clearChanged() override
  {
    SettingsBase::clearChanged();
    rxSettings.clearChanged();
    txSettings.clearChanged();
  }

  static SettingPath getSettingPath(const std::string& strDottedFeatures)
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

  ReceiverSettings rxSettings;
  TransmitterSettings txSettings;

};

#endif //FUNCUBEPLAY_RADIOSETTINGS_H
