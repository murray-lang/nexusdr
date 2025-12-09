//
// Created by murray on 9/12/25.
//

#pragma once
#include "SampleTypes.h"
#include "SettingsBase.h"

class MicSettings : public SettingsBase
{
  public:
  enum Features
  {
    NONE = 0,
    GAIN = 0x01,
  };
  MicSettings(): gain(1.0) {}
  MicSettings(const MicSettings& rhs) = default;
  ~MicSettings() override = default;
  MicSettings& operator=(const MicSettings& rhs) = default;

  bool applySetting(const SingleSetting& setting, int startIndex) override
  {
    if (startIndex >= setting.getPath().getFeatures().size()) {
      throw SettingsException("Invalid setting path");
    }
    bool settingChange = false;
    uint32_t feature = setting.getPath().getFeatures()[startIndex];
    if (feature == GAIN) {
      gain = std::get<sdrreal>(setting.getValue());
      settingChange = true;
    }
    if (settingChange) {
      changed |= feature;
    }
    return settingChange;
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
    if (featureStrings[startIndex] == "gain") {
      features.push_back(GAIN);
    } else {
      throw SettingsException("Unknown RF setting: " + featureStrings[startIndex]);
    }
  }

  sdrreal gain;
};