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
    FINE_STEP = 0x04,
    ALL = static_cast<uint32_t>(~0U)
  };

  PipelineSettings() : m_fineStep(this, "fine-step", false) {}
  PipelineSettings(const PipelineSettings& rhs) :
    SettingsBase(rhs),
    m_mode(rhs.m_mode),
    m_modeSettings(rhs.m_modeSettings),
    m_rfSettings(rhs.m_rfSettings),
    m_fineStep(this, rhs.m_fineStep)
  {
  }
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
      m_rfSettings.applyBandDefaults(*band);
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
    return m_rfSettings.applyBandDefaults(band);
  }

  void setCentreFrequencyDeltas(int32_t fine, int32_t coarse)
  {
    m_rfSettings.setCentreFrequencyDeltas(fine, coarse);
  }

  RfSettings& getRfSettings()
  {
    return m_rfSettings;
  }

  [[nodiscard]] const RfSettings& getRfSettings() const
  {
    return m_rfSettings;
  }

  bool mergeRfSettings(const RfSettings& settings, bool onlyChanged = false)
  {
    if (m_rfSettings.merge(settings, onlyChanged)) {
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

  bool applyUpdate(SettingUpdate& update) override
  {
    if (update.isExhausted()) {
      throw SettingsException("Invalid setting path");
    }
    uint32_t feature = update.getCurrentFeature();

    switch (feature) {
    case RF:
      update.stepNextFeature();
      if (m_rfSettings.applyUpdate(update)) {
        m_changed |= RF;
        return true;
      }
      return false;
    case MODE:
      update.stepNextFeature();
      if (applyModeSetting(update)) {
        m_changed |= MODE;
        return true;
      }
      return false;
    case FINE_STEP: {
      const auto& val = update.getValue();
      return m_fineStep.apply(val);
    }
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
     if (key == "mode") {
      featuresOut.push_back(MODE);
      return true;
    }
    return false;
  }

protected:

  bool applyModeSetting( SettingUpdate& setting)
  {
    if (m_modeSettings.applyUpdate(setting)) {
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
  Setting<bool, FINE_STEP, PipelineSettings> m_fineStep;
};
