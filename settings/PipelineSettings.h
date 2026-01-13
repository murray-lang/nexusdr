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
      m_mode = rhs.m_mode;
      m_rfSettings = rhs.m_rfSettings;
    }
    return *this;
  }

  void clearChanged() override
  {
    SettingsBase::clearChanged();
    m_rfSettings.clearChanged();
  }

  void setAllChanged() override
  {
    SettingsBase::setAllChanged();
    m_rfSettings.setAllChanged();
  }


  void copyBasicsForTracking(const PipelineSettings& rhs)
  {
    m_rfSettings.copyFrequencies(rhs.m_rfSettings);
    m_mode = rhs.m_mode;
    m_changed |= MODE;
  }

  void applyBandDefaults(const Band* band)
  {
    if (band != nullptr) {
      Mode::Type defaultMode = band->getDefaultMode();
      const Mode& newMode = ModeSettings::getModeByType(defaultMode);
      setBand(*band);
      setMode(newMode);
    }
  }

  [[nodiscard]] const Mode& getMode() const { return m_mode; }

  bool setMode( const Mode& newMode )
  {
    if (newMode.getType() != m_mode.getType()) {
      m_mode = newMode;
      m_changed |= MODE;
      return true;
    }
    return false;
  }

  bool setBand(const Band& band)
  {
    return m_rfSettings.setBand(band);
  }

  RfSettings& getRfSettings()
  {
    return m_rfSettings;
  }

  [[nodiscard]] const RfSettings& getRfSettings() const
  {
    return m_rfSettings;
  }

  bool mergeRfSettings(const RfSettings& settings)
  {
    if (m_rfSettings.merge(settings)) {
      m_changed |= RF;
      return true;
    }
    return false;
  }

  bool merge(const PipelineSettings& settings)
  {
    bool somethingChanged = false;

    if (m_rfSettings.merge(settings.m_rfSettings)) {
      m_changed |= RF;
      somethingChanged = true;
    }

    if (settings.m_changed & MODE) {
      if (setMode(settings.m_mode)) {
        m_changed |= MODE;
        somethingChanged = true;
      }
    }
    return somethingChanged;
  }

  bool applyUpdate(const SettingUpdate& update, int startIndex) override
  {
    const auto& features = update.getPath().getFeatures();
    if (startIndex >= features.size()) {
      throw SettingsException("Invalid setting path");
    }

    switch (features[startIndex]) {
    case RF:
      if (m_rfSettings.applyUpdate(update, startIndex + 1)) {
        m_changed |= RF;
        return true;
      }
      return false;
    case MODE:
      if (applyModeSetting(update, startIndex + 1)) {
        m_changed |= MODE;
        return true;
      }
      return false;
    default: return false;
    }
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
    if (resolvePathForRegisteredSetting<PipelineSettings>(featureStrings, featuresOut, startIndex)) {
      return true;
    }
    const std::string& key = featureStrings[startIndex];
    if (key == "rf") {
      featuresOut.push_back(RF);
      return RfSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
    }
    return false;
  }

protected:

  bool applyModeSetting(const SettingUpdate& setting, int index)
  {
    if (m_modeSettings.applyUpdate(setting, index)) {
      const auto& modeType = std::get<Mode::Type>(setting.getValue());
      Mode newMode = ModeSettings::getModeByType(modeType);
      if (newMode.getType() != m_mode.getType()) {
        setMode(newMode);
        return true;
      }
    }
    return false;
  }

  Mode m_mode;
  ModeSettings m_modeSettings;
  RfSettings m_rfSettings;
};
