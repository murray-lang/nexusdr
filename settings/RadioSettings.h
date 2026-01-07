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
#include "BandSettings.h"
#include "SettingsException.h"
#include "../util/StringUtils.h"


class RadioSettings : public SettingsBase {
public:
  enum Features
  {
    NONE = 0,
    PTT = 0x01,
    MODE = 0x02,
    PIPELINE = 0x04,
    TX = 0x08,
    RX = 0x10,
    BAND = 0x20,
    ALL = static_cast<uint32_t>(~0U)
  };

  RadioSettings() : modeType(Mode::Type::NONE),
    ptt(false)
  {};
  RadioSettings(const RadioSettings& rhs) = default;
  
  ~RadioSettings() override = default;

  RadioSettings& operator=(const RadioSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      modeType = rhs.modeType;
      bandName = rhs.bandName;
      rxSettings = rhs.rxSettings;
      txSettings = rhs.txSettings;
    }
    return *this;
  }

  const std::string& getBandName() const { return bandName; }


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
      modeType = std::get<Mode::Type>(setting.getValue());
      changed |= MODE;
      settingChange = true;

    }
    if ((feature & BAND) != 0) {
      const std::string newBandName = std::get<std::string>(setting.getValue());
      if (bandName != newBandName) {
        bandName = newBandName;
        changed |= BAND | PIPELINE;
        settingChange = true;
      }
    }
    if (feature & PIPELINE) {
      changed |= PIPELINE;
      settingChange = true;
    }

    return settingChange;
  }

  void clearChanged() override
  {
    SettingsBase::clearChanged();
    // modeSettings.clearChanged();
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
    } else if (featureStrings[startIndex] == "pipeline") {
      featuresOut.push_back(PIPELINE);
      if (startIndex + 1 < featureStrings.size()) {
        bool validSettingForRxPipeline = true;
        try {
          RxPipelineSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
        } catch (SettingsException& e) {
          validSettingForRxPipeline = false;
        }
        if (!validSettingForRxPipeline) {
          TxPipelineSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
        }
      }
    } else {
      throw SettingsException("Unknown Radio setting: " + featureStrings[startIndex]);
    }

  }
  bool ptt;
  Mode::Type modeType;
  // Band band;
  std::string bandName;
  // Bands bands;
  // ModeSettings modeSettings;
  ReceiverSettings rxSettings;
  TransmitterSettings txSettings;
  // std::unordered_map<std::string, PerBandSettings> perBandSettings; //TODO: Load these from JSON at startup

// protected:
//   PerBandSettings* getCurrentBandSettings()
//   {
//     return findBandSettingsForBand(band.getName());
//   }
//
//   const PerBandSettings* getCurrentBandSettings() const
//   {
//     return findBandSettingsForBand(band.getName());
//   }
//
//   PerBandSettings* findBandSettingsForBand(const std::string& bandName)
//   {
//     if (perBandSettings.contains(bandName)) {
//       return &perBandSettings.at(bandName);
//     }
//     return nullptr;
//   }
//
//   const PerBandSettings* findBandSettingsForBand(const std::string& bandName) const
//   {
//     if (perBandSettings.contains(bandName)) {
//       return &perBandSettings.at(bandName);
//     }
//     return nullptr;
//   }
};
