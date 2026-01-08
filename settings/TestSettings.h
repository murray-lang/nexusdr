//
// Created by murray on 12/12/25.
//

#pragma once
#include "SettingsBase.h"
#include "TwoToneSettings.h"

class TestSettings : public SettingsBase
{
  public:

  enum Features
  {
    NONE = 0,
    TWO_TONE = 0x01,
    ALL = static_cast<uint32_t>(~0U)
  };

  TestSettings() : SettingsBase(), twoToneSettings() {}
  TestSettings(const TestSettings& rhs) = default;
  ~TestSettings() override = default;

  TestSettings& operator=(const TestSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      twoToneSettings = rhs.twoToneSettings;
    }
    return *this;
  }

  bool applySettings(const TestSettings& settings)
  {
    bool somethingChanged = false;
    if (settings.changed & TWO_TONE) {
      twoToneSettings = settings.twoToneSettings;
      changed |= TWO_TONE;
      somethingChanged = true;
    }
    return somethingChanged;
  }


  bool applySetting(const SingleSetting& setting, int startIndex) override
  {
    if (startIndex >= setting.getPath().getFeatures().size()) {
      throw SettingsException("Invalid setting path");
    }
    bool settingChange = false;
    uint32_t feature = setting.getPath().getFeatures()[startIndex];
    if (feature == TWO_TONE) {
      settingChange = twoToneSettings.applySetting(setting, startIndex + 1);
    }
    if (settingChange) {
      changed |= feature;
    }
    return settingChange;
  }

  static bool getFeaturePath(
    const std::vector<std::string>& featureStrings,
    std::vector<uint32_t>& featuresOut,
    size_t startIndex
    )
  {
    if (startIndex >= featureStrings.size()) {
      throw SettingsException("Invalid feature path");
    }
    if (featureStrings[startIndex] == "two-tone") {
      featuresOut.push_back(TWO_TONE);
      if (startIndex + 1 < featureStrings.size()) {
        if (!TwoToneSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1)) {
          featuresOut.pop_back();
          return false;
        }
      }
      return true;
    }
    return false;
  }

  TwoToneSettings twoToneSettings;
};
