//
// Created by murray on 14/04/23.
//

#ifndef FUNCUBEPLAY_IFSETTINGS_H
#define FUNCUBEPLAY_IFSETTINGS_H

#include <cstdint>

#include "SettingsBase.h"

class IfSettings : public SettingsBase {
public:
  enum Features
  {
    NONE = 0,
    BANDWIDTH = 0x01,
    GAIN = 0x02
  };
  IfSettings() : bandwidth(0), gain(0.0) {}
  IfSettings(const IfSettings& rhs) = default;
  ~IfSettings() override = default;

  IfSettings& operator=(const IfSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      bandwidth = rhs.bandwidth;
      gain = rhs.gain;
      changed = rhs.changed;
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
    if (feature == BANDWIDTH) {
      if (setting.getMeaning() == SingleSetting::VALUE) {
        bandwidth = static_cast<uint32_t>(setting.getValue());
        settingChange = true;
      } else if (setting.getMeaning() == SingleSetting::DELTA) {
        bandwidth += setting.getValue();
        settingChange = true;
      }
    } else if (feature == GAIN) {
      if (setting.getMeaning() == SingleSetting::VALUE) {
        gain = static_cast<float>(setting.getValue());
        settingChange = true;
      } else if (setting.getMeaning() == SingleSetting::DELTA) {
        gain += static_cast<float>(setting.getValue());
        settingChange = true;
      }
    }
    if (settingChange) {
      changed |= feature;
    }
    return settingChange;
  }

  void clearChanged() override
  {
    SettingsBase::clearChanged();
  }

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

  uint32_t bandwidth;
  float gain;
};
#endif //FUNCUBEPLAY_IFSETTINGS_H
