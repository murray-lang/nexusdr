//
// Created by murray on 15/04/23.
//

#pragma once


#include "ModeSettings.h"
#include "ReceiverSettings.h"
#include "TransmitterSettings.h"
#include "SettingPath.h"
#include <cstdint>
#include <string>
#include <vector>

#include "Band.h"
#include "Bands.h"
#include "SettingsException.h"
#include "../util/StringUtils.h"


class RadioSettings : public SettingsBase {
public:
  enum Features
  {
    NONE = 0,
    PTT = 0x01,
    MODE = 0x02,
    FREQUENCY = 0x04,
    OFFSET = 0x08,
    TX = 0x10,
    RX = 0x20,
    BAND = 0x40
  };

  RadioSettings() :
  ptt(false), mode(), band(), bands(), modeSettings(), rxSettings(bands, modeSettings), txSettings(bands, modeSettings)
  {};
  RadioSettings(const RadioSettings& rhs) = default;
  
  ~RadioSettings() override = default;

  RadioSettings& operator=(const RadioSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      mode = rhs.mode;
      band = rhs.band;
      rxSettings = rhs.rxSettings;
      txSettings = rhs.txSettings;
    }
    return *this;
  }

  ModeSettings& getModeSettings() { return modeSettings; }

  bool applySetting(const SingleSetting& setting, int startIndex) override
  {
    if (startIndex >= setting.getPath().getFeatures().size()) {
      throw SettingsException("Invalid setting path");
    }
    bool settingChange = false;
    uint32_t feature = setting.getPath().getFeatures()[startIndex];

    if ((feature & PTT) != 0) {
      ptt = std::get<bool>(setting.getValue()) != 0;
      changed |= PTT;
      return true; // PTT shouldn't be combined with anything else. Just return now.
    }

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
        mode = modeSettings.getCurrentMode();
        txSettings.setMode(mode);
        rxSettings.setMode(mode);
        changed |= MODE;
        settingChange = true;
      }
    }
    if ((feature & BAND) != 0) {
      const std::string bandName = std::get<std::string>(setting.getValue());
      const Band* newBand = bands.findBand(bandName);
      if (newBand != nullptr) {
        if (band.getName() != newBand->getName()) {
          band = *newBand;
          changed |= BAND;
          if (txSettings.setBand(band)) {
            changed |= TX;
          }
          if (rxSettings.setBand(band)) {
            changed |= RX;
          }
          settingChange = true;
        }
      }

    } else {
      if ((feature & FREQUENCY) != 0) {
        SingleSetting frequencySetting(setting);
        frequencySetting.setPath(SettingPath({ReceiverSettings::RF, RfSettings::FREQUENCY}));
        if (rxSettings.applySetting(frequencySetting, 0)) {
          changed |= RX | FREQUENCY;
          band = rxSettings.band;
          settingChange = true;
        }
        frequencySetting.setPath(SettingPath({TransmitterSettings::RF, RfSettings::FREQUENCY}));
        if (txSettings.applySetting(frequencySetting, 0)) {
          changed |= TX | FREQUENCY;
          band = rxSettings.band;
          settingChange = true;
        }
      }
      if ((feature & OFFSET) != 0) {
        SingleSetting offsetSetting(setting);
        offsetSetting.setPath(SettingPath({ReceiverSettings::RF, RfSettings::OFFSET}));
        if (rxSettings.applySetting(offsetSetting, 0)) {
          changed |= RX | OFFSET;
          settingChange = true;
        }
        offsetSetting.setPath(SettingPath({TransmitterSettings::RF, RfSettings::OFFSET}));
        if (txSettings.applySetting(offsetSetting, 0)) {
          changed |= TX | OFFSET;
          settingChange = true;
        }
      }
    }


    return settingChange;
  }

  void clearChanged() override
  {
    SettingsBase::clearChanged();
    modeSettings.clearChanged();
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
     if (featureStrings[startIndex] == "ptt") {
      featuresOut.push_back(PTT);
     
    } else if (featureStrings[startIndex] == "rx") {
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
  bool ptt;
  Mode mode;
  Band band;
  Bands bands;
  ModeSettings modeSettings;
  ReceiverSettings rxSettings;
  TransmitterSettings txSettings;
};
