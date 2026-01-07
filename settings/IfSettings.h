//
// Created by murray on 14/04/23.
//

#pragma once

#include <cstdint>

#include "SettingsBase.h"

class IfSettings : public SettingsBase {
public:
  enum Features
  {
    NONE = 0,
    BANDWIDTH = 0x01,
    GAIN = 0x02,
    ALL = static_cast<uint32_t>(~0U)
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

  bool applySettings(const IfSettings& settings)
  {
    bool somethingChanged = false;
    if (settings.changed & BANDWIDTH) {
      bandwidth = settings.bandwidth;
      changed |= BANDWIDTH;
      somethingChanged = true;
    } else if (settings.changed & GAIN) {
      gain = settings.gain;
      changed |= GAIN;
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
    if (feature == BANDWIDTH) {
      if (setting.getMeaning() == SingleSetting::VALUE) {
        bandwidth = std::get<uint32_t>(setting.getValue());
        settingChange = true;
      } else if (setting.getMeaning() == SingleSetting::DELTA) {
        bandwidth += std::get<uint32_t>(setting.getValue());
        settingChange = true;
      }
    } else if (feature == GAIN) {
      if (setting.getMeaning() == SingleSetting::VALUE) {
        gain = std::get<float>(setting.getValue());
        settingChange = true;
      } else if (setting.getMeaning() == SingleSetting::DELTA) {
        gain += std::get<float>(setting.getValue());
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

