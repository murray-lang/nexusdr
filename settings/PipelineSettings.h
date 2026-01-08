//
// Created by murray on 5/1/26.
//

#pragma once
#include "RfSettings.h"
#include "SettingsBase.h"
#include "ModeSettings.h"
#include <map>

class PipelineSettings: public SettingsBase
{
public:
  enum Features
  {
    NONE = 0,
    MODE = 0x01,
    RF = 0x02,
    ALL = static_cast<uint32_t>(~0U)
  };

  PipelineSettings() =default;
  // PipelineSettings(const PipelineSettings& rhs);
  ~PipelineSettings() override = default;

  PipelineSettings& operator=(const PipelineSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      mode = rhs.mode;
      rfSettings = rhs.rfSettings;
    }
    return *this;
  }

  void clearChanged() override
  {
    SettingsBase::clearChanged();
    rfSettings.clearChanged();
  }

  void setAllChanged() override
  {
    SettingsBase::setAllChanged();
    rfSettings.setAllChanged();
  }


  void copyBasicsForTracking(const PipelineSettings& rhs)
  {
    mode = rhs.mode;
    rfSettings.copyFrequencies(rhs.rfSettings);
    changed |= MODE | RF;
  }

  void applyBandDefaults(const Band* band)
  {
    if (band != nullptr) {
      Mode::Type defaultMode = band->getDefaultMode();
      const Mode& newMode = modeSettings.getModeByType(defaultMode);
      setBand(*band);
      setMode(newMode);
    }
  }

  bool setMode( const Mode& newMode )
  {
    if (newMode.getType() != mode.getType()) {
      mode = newMode;
      changed |= MODE;
      return true;
    }
    return false;
  }

  bool setBand(const Band& band)
  {
    if (rfSettings.setBand(band)) {
      changed |= RF;
      return true;
    }
    return false;
  }

  RfSettings& getRfSettings()
  {
    return rfSettings;
  }

  void applyRfSettings(const RfSettings& settings)
  {
    rfSettings.applySettings(settings);
  }

  bool applySettings(const PipelineSettings& settings)
  {
    bool somethingChanged = false;
    if (settings.changed & RF) {
      if (rfSettings.applySettings(settings.rfSettings)) {
        changed |= RF;
        somethingChanged = true;
      }
    }
    if (settings.changed & MODE) {
      if (setMode(settings.mode)) {
        somethingChanged = true;
      }
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

    using Handler = bool (PipelineSettings::*)(const SingleSetting&, int);
    struct HandlerEntry { Features feature; Handler method; };

    static constexpr HandlerEntry dispatchTable[] = {
    {RF, &PipelineSettings::applyRfSetting },
    {MODE, &PipelineSettings::applyModeSetting }
    };

    for (const auto& h : dispatchTable) {
      if (feature & h.feature) {
        if ((this->*h.method)(setting, startIndex)) {
          settingChange = true;
          changed |= h.feature;
        }
      }
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
    const std::string& key = featureStrings[startIndex];

    using PathFunc = bool(*)(const std::vector<std::string>&, std::vector<uint32_t>&, size_t);
    static const std::map<std::string, std::pair<Features, PathFunc>> dispatch = {
      {"rf",         {RF,         &RfSettings::getFeaturePath}},
      {"mode",       {MODE,       &SettingsBase::addFeature<MODE>}},
    };

    if (auto it = dispatch.find(key); it != dispatch.end()) {
      featuresOut.push_back(it->second.first);
      if (startIndex + 1 < featureStrings.size() && it->second.second) {
        if (!it->second.second(featureStrings, featuresOut, startIndex + 1)) {
          featuresOut.pop_back();
          return false;
        }
      }
      return true;
    }
    return false;
  }

  Mode mode;
  ModeSettings modeSettings;
  RfSettings rfSettings;

protected:
  bool applyRfSetting(const SingleSetting& setting, int index)
  {
    return rfSettings.applySetting(setting, index);
  }

  bool applyModeSetting(const SingleSetting& setting, int index)
  {
    if (modeSettings.applySetting(setting, index)) {
    }
    const auto& modeType = std::get<Mode::Type>(setting.getValue());
    Mode newMode = modeSettings.getModeByType(modeType);
    if (newMode.getType() != mode.getType()) {
      setMode(newMode);
      return true;
    }
    return false;
  }
};
