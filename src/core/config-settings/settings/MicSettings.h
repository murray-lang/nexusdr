//
// Created by murray on 9/12/25.
//

#pragma once
#include "../../SampleTypes.h"
#include "base/SettingsBase.h"

#define DEFAULT_GAIN_STEP 1.0

class MicSettings : public SettingsBase
{
  public:
  enum Features
  {
    NONE = 0,
    GAIN = 0x01,
    GAIN_STEP,
    ALL = static_cast<uint32_t>(~0U)
  };
  MicSettings():
    m_gain(this, "gain",1.0),
    m_gainStep(this, "gain-step", DEFAULT_GAIN_STEP)
  {
    m_gain.setFineStepValueAddress(m_gainStep.getValueAddress());
  }
  MicSettings(const MicSettings& rhs) :
    SettingsBase(rhs),
    m_gain(this, rhs.m_gain),
    m_gainStep(this, rhs.m_gainStep)
  {
    m_gain.setFineStepValueAddress(m_gainStep.getValueAddress());
  }
  ~MicSettings() override = default;
  MicSettings& operator=(const MicSettings& rhs) = default;

  [[nodiscard]] sdrreal getGain() const { return m_gain(); }
  [[nodiscard]] sdrreal getGainStep() const { return m_gainStep(); }

  bool merge(const MicSettings& settings)
  {
    return m_gain.merge(settings.m_gain) | m_gainStep.merge(settings.m_gainStep);
  }

  bool applyUpdate(SettingUpdate& update) override
  {
    if (update.isExhausted()) {
      throw SettingsException("Invalid setting path");
    }
    uint32_t feature = update.getCurrentFeature();
    const auto& val = update.getValue();

    switch (feature) {
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
    return resolvePathForRegisteredSetting<MicSettings>(featureStrings, out, startIndex);
  }

protected:
  Setting<sdrreal, GAIN, MicSettings> m_gain;
  Setting<sdrreal, GAIN_STEP, MicSettings> m_gainStep;
};