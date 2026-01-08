//
// Created by murray on 14/04/23.
//

#pragma once


#include "Band.h"
#include "RfSettings.h"
#include "IfSettings.h"
#include "IqCorrectionSettings.h"
#include "SettingsException.h"
#include "Mode.h"
#include "Band.h"
#include "Bands.h"
#include "ModeSettings.h"
#include <map>

class ReceiverSettings : public SettingsBase {
public:
  enum Features
  {
    NONE = 0,
    // MODE = 0x01,
    // RF = 0x02,
    // IF = 0x04,
    CORRECTION = 0x08,
    // BAND= 0x10
    ALL = static_cast<uint32_t>(~0U)
  };

  explicit ReceiverSettings(/*const Bands& bands, const ModeSettings& modeSettings*/) :
    SettingsBase() //, mode(), modeSettings(modeSettings), band(), bands(bands), rfSettings(), ifSettings()
  {};
  ReceiverSettings(const ReceiverSettings& rhs) = default;

  ~ReceiverSettings() override = default;

  ReceiverSettings& operator=(const ReceiverSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      correctionSettings = rhs.correctionSettings;
    }
    return *this;
  }

  bool applySettings(const ReceiverSettings& settings)
  {
    bool somethingChanged = false;
    if (settings.changed & CORRECTION) {
      correctionSettings = settings.correctionSettings;
      changed |= CORRECTION;
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
    // if (feature & RF) {
    //   settingChange = rfSettings.applySetting(setting, startIndex + 1);
    //   if (rfSettings.changed & RfSettings::FREQUENCY || rfSettings.changed & RfSettings::OFFSET) {
    //     uint64_t frequency = rfSettings.frequency + rfSettings.offset;
    //     if (!band.isValid() || !band.containsFrequency(frequency)) {
    //       const Band* newBand = bands.findBand(frequency);
    //       if (newBand != nullptr) {
    //         band = *newBand;
    //       } else {
    //         band.invalidate();
    //       }
    //       changed |= BAND;
    //     }
    //   }
    //
    // } else if (feature & IF) {
    //   settingChange = ifSettings.applySetting(setting, startIndex + 1);
    // } else
    if (feature == CORRECTION) {
      settingChange = correctionSettings.applySetting(setting, startIndex + 1);
    }
    if (settingChange) {
      changed |= feature;
    }
    return settingChange;
  }

  void clearChanged() override
  {
    SettingsBase::clearChanged();
    // rfSettings.clearChanged();
    // ifSettings.clearChanged();
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
    const std::string& key = featureStrings[startIndex];

    using PathFunc = bool(*)(const std::vector<std::string>&, std::vector<uint32_t>&, size_t);
    static const std::map<std::string, std::pair<Features, PathFunc>> dispatch = {
      {"correction", {CORRECTION, &IqCorrectionSettings::getFeaturePath}}
    };

    if (auto it = dispatch.find(key); it != dispatch.end()) {
      featuresOut.push_back(it->second.first);
      if (startIndex + 1 < featureStrings.size() && it->second.second) {
        if(!it->second.second(featureStrings, featuresOut, startIndex + 1)) {
          featuresOut.pop_back();
          return false;
        }
      }
      return true;
    }
    return false;
  }
  IqCorrectionSettings correctionSettings;
};

