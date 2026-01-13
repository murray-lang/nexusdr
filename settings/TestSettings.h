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

  TestSettings() : SettingsBase(), m_twoToneSettings() {}
  TestSettings(const TestSettings& rhs) = default;
  ~TestSettings() override = default;

  TestSettings& operator=(const TestSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      m_twoToneSettings = rhs.m_twoToneSettings;
    }
    return *this;
  }

  TwoToneSettings& getTwoToneSettings() { return m_twoToneSettings; }

  bool merge(const TestSettings& settings)
  {
    return m_twoToneSettings.merge(settings.m_twoToneSettings);
  }


  bool applyUpdate(const SettingUpdate& setting, int startIndex) override
  {
    if (startIndex >= setting.getPath().getFeatures().size()) {
      throw SettingsException("Invalid setting path");
    }
    uint32_t feature = setting.getPath().getFeatures()[startIndex];
    if (feature == TWO_TONE) {
      if (m_twoToneSettings.applyUpdate(setting, startIndex + 1)) {
        m_changed |= TWO_TONE;
        return true;
      }
    }

    return false;
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
      return TwoToneSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
    }
    return false;
  }
protected:
  TwoToneSettings m_twoToneSettings;
};
