//
// Created by murray on 15/04/23.
//

#ifndef FUNCUBEPLAY_RADIOSETTINGS_H
#define FUNCUBEPLAY_RADIOSETTINGS_H

#include "ModeSettings.h"
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
    MODE = 0x01,
    FREQUENCY = 0x02,
    OFFSET = 0x04,
    TX = 0x08,
    RX = 0x10//,
    // TXRX = TX | RX
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
    if ((feature & TX) != 0) {
      if (txSettings.applySetting(setting, startIndex + 1)) {
        changed |= TX;
        settingChange = true;
      }
    }
    if ((feature & RX) != 0) {
      if (rxSettings.applySetting(setting, startIndex + 1)) {
        changed |= RX;
        settingChange = true;
      }
    }
    if ((feature & MODE) != 0) {
      if (modeSettings.applySetting(setting, startIndex)) {
        const Mode& mode = modeSettings.getCurrentMode();
        txSettings.setMode(mode);
        rxSettings.setMode(mode);
        changed |= MODE;
        settingChange = true;
      }

    }
    if ((feature & FREQUENCY) != 0) {
      SingleSetting frequencySetting(setting);
      frequencySetting.setPath(SettingPath({ReceiverSettings::RF, RfSettings::FREQUENCY}));
      if (rxSettings.applySetting(frequencySetting, 0)) {
        changed |= FREQUENCY;
        settingChange = true;
      }
      frequencySetting.setPath(SettingPath({TransmitterSettings::RF, RfSettings::FREQUENCY}));
      if (txSettings.applySetting(frequencySetting, 0)) {
        changed |= FREQUENCY;
        settingChange = true;
      }
    }
    if ((feature & OFFSET) != 0) {
      SingleSetting offsetSetting(setting);
      offsetSetting.setPath(SettingPath({ReceiverSettings::RF, RfSettings::OFFSET}));
      if (rxSettings.applySetting(offsetSetting, 0)) {
        changed |= OFFSET;
        settingChange = true;
      }
      offsetSetting.setPath(SettingPath({TransmitterSettings::RF, RfSettings::OFFSET}));
      if (txSettings.applySetting(offsetSetting, 0)) {
        changed |= OFFSET;
        settingChange = true;
      }
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
    } else if (featureStrings[startIndex] == "mode") {
      featuresOut.push_back(MODE);
    } else if (featureStrings[startIndex] == "frequency") {
      featuresOut.push_back(FREQUENCY);
    } else if (featureStrings[startIndex] == "offset") {
      featuresOut.push_back(OFFSET);
    } else {
      throw SettingsException("Unknown Radio setting: " + featureStrings[startIndex]);
    }

  }

  ModeSettings modeSettings;
  ReceiverSettings rxSettings;
  TransmitterSettings txSettings;

};

#endif //FUNCUBEPLAY_RADIOSETTINGS_H
