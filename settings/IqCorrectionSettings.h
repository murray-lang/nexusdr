//
// Created by murray on 28/11/25.
//

#pragma once
#include "SampleTypes.h"
#include "SettingsBase.h"

#define DEFAULT_STEP_SIZE 0.001

class IqCorrectionSettings : public SettingsBase
{
public:
  enum Features
  {
    NONE = 0,
    PHASE = 0x01,
    PHASE_STEP = 0x02,
    AMPLITUDE = 0x04,
    AMPLITUDE_STEP = 0x08,
    ALL = static_cast<uint32_t>(~0U)
  };
  IqCorrectionSettings() : phase(0.0), phaseStep(DEFAULT_STEP_SIZE), amplitude(0.0), amplitudeStep(DEFAULT_STEP_SIZE) {}
  IqCorrectionSettings(const IqCorrectionSettings& rhs) = default;
  ~IqCorrectionSettings() override = default;

  IqCorrectionSettings& operator=(const IqCorrectionSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      phase = rhs.phase;
      amplitude = rhs.amplitude;
    }
    return *this;
  }

  bool applySettings(const IqCorrectionSettings& settings)
  {
    bool somethingChanged = false;
    if (settings.changed & PHASE) {
      phase = settings.phase;
      changed |= PHASE;
      somethingChanged = true;
    }
    if (settings.changed & PHASE_STEP) {
      phaseStep = settings.phaseStep;
      changed |= PHASE_STEP;
      somethingChanged = true;
    }
    if (settings.changed & AMPLITUDE) {
      amplitude = settings.amplitude;
      changed |= AMPLITUDE;
      somethingChanged = true;
    }
    if (settings.changed & AMPLITUDE_STEP) {
      amplitudeStep = settings.amplitudeStep;
      changed |= AMPLITUDE_STEP;
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
    if (feature == PHASE) {
      if (setting.getMeaning() == SingleSetting::VALUE) {
        phase = std::get<sdrreal>(setting.getValue());
        settingChange = true;
      } else if (setting.getMeaning() == SingleSetting::DELTA) {
        phase += static_cast<sdrreal>(std::get<int32_t>(setting.getValue())) * phaseStep;
        settingChange = true;
      }
    } if (feature == PHASE_STEP) {
      if (setting.getMeaning() == SingleSetting::VALUE) {
        phaseStep = std::get<sdrreal>(setting.getValue());
        settingChange = true;
      } else if (setting.getMeaning() == SingleSetting::DELTA) {
        phaseStep += static_cast<sdrreal>(std::get<int32_t>(setting.getValue())) * 10;
        settingChange = true;
      }
    } else if (feature == AMPLITUDE) {
      if (setting.getMeaning() == SingleSetting::VALUE) {
        amplitude = std::get<sdrreal>(setting.getValue());
        settingChange = true;
      } else if (setting.getMeaning() == SingleSetting::DELTA) {
        amplitude += static_cast<sdrreal>(std::get<int32_t>(setting.getValue())) * amplitudeStep;
        settingChange = true;
      }
    } else if (feature == AMPLITUDE_STEP) {
      if (setting.getMeaning() == SingleSetting::VALUE) {
        amplitudeStep = std::get<sdrreal>(setting.getValue());
        settingChange = true;
      } else if (setting.getMeaning() == SingleSetting::DELTA) {
        amplitudeStep += static_cast<sdrreal>(std::get<int32_t>(setting.getValue())) * 10;
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
    if (featureStrings[startIndex] == "phase") {
      features.push_back(PHASE);
    } else if (featureStrings[startIndex] == "phase-step") {
      features.push_back(PHASE_STEP);
    } else if (featureStrings[startIndex] == "amplitude") {
      features.push_back(AMPLITUDE);
    } else if (featureStrings[startIndex] == "amplitude-step") {
      features.push_back(AMPLITUDE_STEP);
    } else {
      throw SettingsException("Unknown IQ correction setting: " + featureStrings[startIndex]);
    }
  }

  sdrreal phase;
  sdrreal phaseStep;
  sdrreal amplitude;
  sdrreal amplitudeStep;
};