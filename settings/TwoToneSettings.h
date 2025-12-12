//
// Created by murray on 12/12/25.
//

#pragma once
#include "SampleTypes.h"
#include "SettingsBase.h"

#define DEFAULT_GAIN 1.0
#define DEFAULT_GAIN_STEP 1.0

class TwoToneSettings : SettingsBase
{
public:
  enum Features
  {
    NONE = 0,
    ENABLED  = 0x01,
    GAIN   = 0x02,
  };
  TwoToneSettings() : enabled(false), gain(DEFAULT_GAIN), gainStep(DEFAULT_GAIN_STEP) {}
  TwoToneSettings(const TwoToneSettings& rhs) = default;
  ~TwoToneSettings() override  = default;

  TwoToneSettings& operator=(const TwoToneSettings& rhs) = default;

  bool applySetting(const SingleSetting& setting, int startIndex) override
  {
    if (startIndex >= setting.getPath().getFeatures().size()) {
      throw SettingsException("Invalid setting path");
    }
    bool settingChange = false;
    uint32_t feature = setting.getPath().getFeatures()[startIndex];
    if (feature == ENABLED) {
      enabled = !enabled;
    } else if (feature == GAIN) {
      if (setting.getMeaning() == SingleSetting::VALUE) {
        gain = std::get<float>(setting.getValue());
        settingChange = true;
      } else if (setting.getMeaning() == SingleSetting::DELTA) {
        gain += static_cast<sdrreal>(std::get<int32_t>(setting.getValue())) * gainStep;
        settingChange = true;
      }
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
    if (featureStrings[startIndex] == "enabled") {
      features.push_back(ENABLED);
    } else if (featureStrings[startIndex] == "gain") {
      features.push_back(GAIN);
    } else {
      throw SettingsException("Unknown TwoTone setting: " + featureStrings[startIndex]);
    }
  }

  bool enabled;
  sdrreal gain;
  sdrreal gainStep;

};