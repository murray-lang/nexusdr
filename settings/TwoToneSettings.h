//
// Created by murray on 12/12/25.
//

#pragma once
#include "SampleTypes.h"
#include "core/SettingsBase.h"

#define DEFAULT_GAIN 1.0
#define DEFAULT_GAIN_STEP 1.0

class TwoToneSettings : public SettingsBase
{
public:
  enum Features
  {
    NONE = 0,
    ENABLED  = 0x01,
    GAIN   = 0x02,
    GAIN_STEP = 0x04,
    ALL = static_cast<uint32_t>(~0U)
  };
  TwoToneSettings() :
    m_enabled(this, "enabled",false),
    m_gain(this, "gain", DEFAULT_GAIN),
    m_gainStep(this, "gain_step", DEFAULT_GAIN_STEP)
  {
    m_gain.setFineStepValueAddress(m_gainStep.getValueAddress());
  }
  TwoToneSettings(const TwoToneSettings& rhs) :
    SettingsBase(rhs),
    m_enabled(this, rhs.m_enabled),
    m_gain(this, rhs.m_gain),
    m_gainStep(this, rhs.m_gainStep)
  {
    m_gain.setFineStepValueAddress(m_gainStep.getValueAddress());
  }
  ~TwoToneSettings() override  = default;

  TwoToneSettings& operator=(const TwoToneSettings& rhs) = default;

  bool merge(const TwoToneSettings& settings)
  {
    return m_enabled.merge(settings.m_enabled)
      | m_gain.merge(settings.m_gain)
      | m_gainStep.merge(settings.m_gainStep);
  }

  bool applyUpdate(SettingUpdate& update) override
  {
    if (update.isExhausted()) {
      throw SettingsException("Invalid setting path");
    }
    uint32_t feature = update.getCurrentFeature();
    const auto& val = update.getValue();

    switch (feature) {
    case ENABLED: return m_enabled.apply(update);
    case GAIN: return m_gain.apply(update);
    case GAIN_STEP: return m_gainStep.apply(val);
    default: return false;
    }
  }

  static bool getFeaturePath(
    const std::vector<std::string>& featureStrings,
    std::vector<uint32_t>& out,
    size_t startIndex
    )
  {
    return resolvePathForRegisteredSetting<TwoToneSettings>(featureStrings, out, startIndex);
  }
protected:
  Setting<bool, ENABLED, TwoToneSettings> m_enabled;
  Setting<sdrreal, GAIN, TwoToneSettings> m_gain;
  Setting<sdrreal, GAIN_STEP, TwoToneSettings>   m_gainStep;

};