//
// Created by murray on 14/04/23.
//

#pragma once

#include "IqCorrectionSettings.h"
#include "RfSettings.h"

class TransmitterSettings : public SettingsBase {
public:
  enum Features
  {
    NONE = 0,
    MODE = 0x01,
    RF = 0x02,
    CORRECTION = 0x04
  };
  TransmitterSettings() = default;
  TransmitterSettings(const TransmitterSettings& rhs) = default;
  ~TransmitterSettings() override = default;

  TransmitterSettings& operator=(const TransmitterSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      rfSettings = rhs.rfSettings;
      mode = rhs.mode;
    }
    return *this;
  }
  void setMode( const Mode& newMode )
  {
    mode = newMode;
    changed |= MODE;
  }
  [[nodiscard]] const Mode& getMode() const { return mode; }

  bool applySetting(const SingleSetting& setting, int startIndex) override
  {
    if (startIndex >= setting.getPath().getFeatures().size()) {
      throw SettingsException("Invalid setting path");
    }
    bool settingChange = false;
    uint32_t feature = setting.getPath().getFeatures()[startIndex];
    if (feature == RF) {
      settingChange = rfSettings.applySetting(setting, startIndex + 1);
    } else if (feature == CORRECTION) {
      settingChange = correctionSettings.applySetting(setting, startIndex + 1);
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
    } else if (featureStrings[startIndex] == "correction") {
      featuresOut.push_back(CORRECTION);
      if (startIndex + 1 < featureStrings.size()) {
        IqCorrectionSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
      }
    } else {
      throw SettingsException("Unknown transmitter feature: " + featureStrings[startIndex]);
    }
  }

  Mode mode;
  RfSettings rfSettings;
  IqCorrectionSettings correctionSettings;
};
