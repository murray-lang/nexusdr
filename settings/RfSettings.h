//
// Created by murray on 14/04/23.
//

#pragma once


#include <cstdint>

#include "SettingsBase.h"
#include "SettingsException.h"

class RfSettings : public SettingsBase
{
public:
  enum Features
  {
    NONE = 0,
    FREQUENCY = 0x01,
    FREQUENCY_STEP = 0x02,
    OFFSET = 0x04,
    OFFSET_STEP = 0x08,
    GAIN = 0x10,

  };
  RfSettings() : frequency(0), frequencyStep(10000), offset(0), offsetStep(200), gain(0.0) {}
  RfSettings(const RfSettings& rhs) = default;
  ~RfSettings() override = default;

  RfSettings& operator=(const RfSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      frequency = rhs.frequency;
      frequencyStep = rhs.frequencyStep;
      offset = rhs.offset;
      offsetStep = rhs.offsetStep;
      gain = rhs.gain;
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
    if (feature == FREQUENCY) {
      if (setting.getMeaning() == SingleSetting::VALUE) {
        frequency = std::get<uint32_t>(setting.getValue());
        settingChange = true;
      } else if (setting.getMeaning() == SingleSetting::DELTA) {
        frequency += std::get<int32_t>(setting.getValue()) * frequencyStep;
        settingChange = true;
      }
    } if (feature == OFFSET) {
      if (setting.getMeaning() == SingleSetting::VALUE) {
        offset = std::get<int32_t>(setting.getValue());
        settingChange = true;
      } else if (setting.getMeaning() == SingleSetting::DELTA) {
        offset += std::get<int32_t>(setting.getValue()) * offsetStep;
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
    if (featureStrings[startIndex] == "frequency") {
      features.push_back(FREQUENCY);
    } else if (featureStrings[startIndex] == "gain") {
      features.push_back(GAIN);
    } else if (featureStrings[startIndex] == "offset") {
      features.push_back(OFFSET);
    } else {
      throw SettingsException("Unknown RF setting: " + featureStrings[startIndex]);
    }
  }

  uint32_t frequency;
  uint32_t frequencyStep;
  int32_t  offset;
  uint32_t offsetStep;
  float gain;
};
