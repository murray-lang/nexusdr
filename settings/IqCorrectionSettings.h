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
  IqCorrectionSettings() :
    m_phase(this, "phase",0.0),
    m_phaseStep(this, "phase-step", DEFAULT_STEP_SIZE),
    m_amplitude(this, "amplitude", 0.0),
    m_amplitudeStep(this, "amplitude-step", DEFAULT_STEP_SIZE)
  {
    m_phase.setStepValueAddress(m_phaseStep.getValueAddress());
    m_amplitude.setStepValueAddress(m_amplitudeStep.getValueAddress());
  }
  IqCorrectionSettings(const IqCorrectionSettings& rhs) :
    SettingsBase(rhs),
    m_phase(this, rhs.m_phase),
    m_phaseStep(this, rhs.m_phaseStep),
    m_amplitude(this, rhs.m_amplitude),
    m_amplitudeStep(this, rhs.m_amplitudeStep)
  {
    m_phase.setStepValueAddress(m_phaseStep.getValueAddress());
    m_amplitude.setStepValueAddress(m_amplitudeStep.getValueAddress());
  }
  ~IqCorrectionSettings() override = default;

  IqCorrectionSettings& operator=(const IqCorrectionSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      m_phase = rhs.m_phase;
      m_phaseStep = rhs.m_phaseStep;
      m_amplitude = rhs.m_amplitude;
      m_amplitudeStep = rhs.m_amplitudeStep;
    }
    return *this;
  }

  [[nodiscard]] sdrreal getPhase() const { return m_phase(); }
  [[nodiscard]] sdrreal getPhaseStep() const { return m_phaseStep(); }
  [[nodiscard]] sdrreal getAmplitude() const { return m_amplitude(); }
  [[nodiscard]] sdrreal getAmplitudeStep() const { return m_amplitudeStep(); }

  bool merge(const IqCorrectionSettings& rhs)
  {
    return m_phase.merge(rhs.m_phase)
    | m_phaseStep.merge(rhs.m_phaseStep)
    | m_amplitude.merge(rhs.m_amplitude)
    | m_amplitudeStep.merge(rhs.m_amplitudeStep);
  }

  bool applyUpdate(SettingUpdate& update) override
  {
    if (update.isExhausted()) {
      throw SettingsException("Invalid setting path");
    }
    uint32_t feature = update.getCurrentFeature();
    const auto& val = update.getValue();

    switch (feature) {
    case PHASE: return m_phase.apply(update);
    case PHASE_STEP: return m_phaseStep.apply(val);
    case AMPLITUDE: return m_amplitude.apply(update);
    case AMPLITUDE_STEP: return m_amplitudeStep.apply(val);
    default: return false;
    }
  }
  
  static bool getFeaturePath(
    const std::vector<std::string>& featureStrings,
    std::vector<uint32_t>& out,
    size_t startIndex
    )
  {
    return resolvePathForRegisteredSetting<IqCorrectionSettings>(featureStrings, out, startIndex);
  }

protected:

  Setting<sdrreal, PHASE, IqCorrectionSettings> m_phase;
  Setting<sdrreal, PHASE_STEP, IqCorrectionSettings> m_phaseStep;
  Setting<sdrreal, AMPLITUDE, IqCorrectionSettings> m_amplitude;
  Setting<sdrreal, AMPLITUDE_STEP, IqCorrectionSettings> m_amplitudeStep;
};