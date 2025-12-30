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

class ReceiverSettings : public SettingsBase {
public:
  enum Features
  {
    NONE = 0,
    MODE = 0x01,
    RF = 0x02,
    IF = 0x04,
    CORRECTION = 0x08,
    BAND= 0x10
  };

  explicit ReceiverSettings(const Bands& bands, const ModeSettings& modeSettings) :
    SettingsBase(), mode(), modeSettings(modeSettings), band(), bands(bands), rfSettings(), ifSettings()
  {};
  ReceiverSettings(const ReceiverSettings& rhs) = default;

  ~ReceiverSettings() override = default;

  ReceiverSettings& operator=(const ReceiverSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      mode = rhs.mode;
      rfSettings = rhs.rfSettings;
      ifSettings = rhs.ifSettings;
    }
    return *this;
  }

  void setMode( const Mode& newMode )
  {
    mode = newMode;
    changed |= MODE;
  }
  [[nodiscard]] const Mode& getMode() const { return mode; }

  bool setBand( const Band& newBand)
  {
    if (band.getName() != newBand.getName()) {
      band = newBand;
      changed |= BAND;
      if (band.isValid()) {
        if (rfSettings.setBand(band)) {
          changed |= RF;
          Mode newMode = modeSettings.getModeByType(band.getDefaultMode());
          setMode(newMode);
        }
      }
      return true;
    }
    return false;
  }
  [[nodiscard]] const Band& getBand() const { return band; }

  bool applySetting(const SingleSetting& setting, int startIndex) override
  {
    if (startIndex >= setting.getPath().getFeatures().size()) {
      throw SettingsException("Invalid setting path");
    }
    bool settingChange = false;
    uint32_t feature = setting.getPath().getFeatures()[startIndex];
    if (feature & RF) {
      settingChange = rfSettings.applySetting(setting, startIndex + 1);
      if (rfSettings.changed & RfSettings::FREQUENCY || rfSettings.changed & RfSettings::OFFSET) {
        uint64_t frequency = rfSettings.frequency + rfSettings.offset;
        if (!band.isValid() || !band.containsFrequency(frequency)) {
          const Band* newBand = bands.findBand(frequency);
          if (newBand != nullptr) {
            band = *newBand;
          } else {
            band.invalidate();
          }
          changed |= BAND;
        }
      }

    } else if (feature & IF) {
      settingChange = ifSettings.applySetting(setting, startIndex + 1);
    } else if (feature == CORRECTION) {
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
    rfSettings.clearChanged();
    ifSettings.clearChanged();
  }

  static void getFeaturePath(
    const std::vector<std::string>& featureStrings,
    std::vector<uint32_t>& featuresOut,
    size_t startIndex
    )
  {
    if (startIndex >= featureStrings.size()) {
      throw SettingsException("Invalid feature path");
    }
    if (featureStrings[startIndex] == "rf") {
      featuresOut.push_back(RF);
      if (startIndex + 1 < featureStrings.size()) {
        RfSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
      }
    } else if (featureStrings[startIndex] == "if") {
      featuresOut.push_back(IF);
      if (startIndex + 1 < featureStrings.size()) {
        IfSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
      }
    } else if (featureStrings[startIndex] == "correction") {
      featuresOut.push_back(CORRECTION);
      if (startIndex + 1 < featureStrings.size()) {
        IqCorrectionSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
      }
    } else {
      throw SettingsException("Unknown receiver setting: " + featureStrings[startIndex]);
    }
  }
  Mode mode;
  const ModeSettings& modeSettings;
  Band band;
  const Bands& bands;
  RfSettings rfSettings;
  IfSettings ifSettings;
  IqCorrectionSettings correctionSettings;
};

