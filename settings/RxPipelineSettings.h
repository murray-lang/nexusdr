//
// Created by murray on 5/1/26.
//

#pragma once
#include "IfSettings.h"
#include "PipelineSettings.h"
#include <QDebug>

class RxPipelineSettings : public PipelineSettings
{
public:
  enum Features
  {
    NONE = 0,
    // 0x01 and 0x02 are used by PipelineSettings
    IF = 0x04,
    MUTE = 0x08,
    AGC = 0x10,
    ALL = static_cast<uint32_t>(~0U)
  };
  RxPipelineSettings() :
    PipelineSettings(),
    m_mute(this, "mute", false),
    m_agc(this, "agc", 0.0)
  {
  }

  RxPipelineSettings(const RxPipelineSettings& rhs):
    PipelineSettings(rhs),
    m_mute(this, rhs.m_mute),
    m_agc(this, rhs.m_agc),
    m_ifSettings(rhs.m_ifSettings)
  {
  }
  ~RxPipelineSettings() override = default;

  RxPipelineSettings& operator=(const RxPipelineSettings& rhs)
  {
    if (this != &rhs) {
      PipelineSettings::operator=(rhs);
      m_mute = rhs.m_mute;
      m_ifSettings = rhs.m_ifSettings;
    }
    return *this;
  }

  void clearChanged() override
  {
    PipelineSettings::clearChanged();
    m_ifSettings.clearChanged();
  }

  void setAllChanged() override
  {
    PipelineSettings::setAllChanged();
    m_ifSettings.setAllChanged();
  }

  IfSettings& getIfSettings()
  {
    return m_ifSettings;
  }

  const IfSettings& getIfSettings() const
  {
    return m_ifSettings;
  }

  void mergeIfSettings(const IfSettings& settings)
  {
    m_ifSettings.merge(settings);
  }

  bool merge(const RxPipelineSettings& settings)
  {
    bool changed = PipelineSettings::merge(settings)
    | m_mute.merge(settings.m_mute)
    | m_agc.merge(settings.m_agc);

    if (m_ifSettings.merge(settings.m_ifSettings)) {
      m_changed |= IF;
      return true;
    }
    return changed;
  }

  bool applyUpdate(SettingUpdate& update) override
  {
    if (PipelineSettings::applyUpdate(update)) {
      return true;
    }
    if (update.isExhausted()) {
      throw SettingsException("Invalid setting path");
    }
    uint32_t feature = update.getCurrentFeature();
    const auto& val = update.getValue();

    switch (feature) {
    case MUTE:  return m_mute.apply(val);
    case AGC: return m_agc.apply(val);
    case IF:
      update.stepNextFeature();
      if (m_ifSettings.applyUpdate(update)) {
        m_changed |= IF;
        return true;
      }
      return false;
    default:
      return false;
    }
  }

  static bool getFeaturePath(
    const std::vector<std::string>& featureStrings,
    std::vector<uint32_t>& featuresOut,
    size_t startIndex
    )
  {
    if (PipelineSettings::getFeaturePath(featureStrings, featuresOut, startIndex)) {
      return true;
    }
    if (startIndex >= featureStrings.size()) {
      throw SettingsException("Invalid feature path");
    }

    if (resolvePathForRegisteredSetting<RxPipelineSettings>(featureStrings, featuresOut, startIndex)) {
      return true;
    }

    const std::string& key = featureStrings[startIndex];
    if (key == "if") {
      featuresOut.push_back(IF);
      return RfSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
    }
    return false;
  }

protected:
  Setting<bool, MUTE, RxPipelineSettings> m_mute;
  Setting<float, AGC, RxPipelineSettings> m_agc;
  IfSettings m_ifSettings;
};
