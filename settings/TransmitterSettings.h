//
// Created by murray on 14/04/23.
//

#ifndef FUNCUBEPLAY_TRANSMITTERSETTINGS_H
#define FUNCUBEPLAY_TRANSMITTERSETTINGS_H

#include "RfSettings.h"

class TransmitterSettings : public SettingsBase {
public:
  enum Features
  {
    NONE = 0,
    RF = 0x01
  };
  TransmitterSettings() = default;
  TransmitterSettings(const TransmitterSettings& rhs) = default;
  ~TransmitterSettings() override = default;

  TransmitterSettings& operator=(const TransmitterSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      rfSettings = rhs.rfSettings;
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
    if (feature == RF) {
      settingChange = rfSettings.applySetting(setting, startIndex + 1);
    }
    if (settingChange) {
      changed |= feature;
    }
    return settingChange;
  }

  void clearChanged() override
  {
    SettingsBase::clearChanged();
    rfSettings.clearChanged();
  }

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
    } else {
      throw SettingsException("Unknown transmitter feature: " + featureStrings[startIndex]);
    }
  }

  RfSettings rfSettings;
};

#endif //FUNCUBEPLAY_TRANSMITTERSETTINGS_H
