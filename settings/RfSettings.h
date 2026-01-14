//
// Created by murray on 14/04/23.
//

#pragma once


#include <cstdint>

#include "Bands.h"
#include "SettingsBase.h"
#include "SettingsException.h"
#include <QDebug>

class RfSettings : public SettingsBase
{
public:
  enum Features
  {
    NONE = 0,
    FREQUENCY = 0x01,
    FREQUENCY_STEP = 0x02,
    OFFSET = 0x04,
    OFFSET_STEP = 0x08,
    GAIN = 0x10,
    GAIN_STEP = 0x20,
    ALL = static_cast<uint32_t>(~0U)
  };
  RfSettings() :
    m_frequency(this, "frequency", 0),
    m_frequencyStep(this, "frequency-step", 10000),
    m_offset(this, "offset", 0),
    m_offsetStep(this, "offset-step", 50),
    m_gain(this, "gain", 0.0),
    m_gainStep(this, "gain-step", 0.0)
  {
    m_frequency.setStepValueAddress(m_frequencyStep.getValueAddress());
    m_offset.setStepValueAddress(m_offsetStep.getValueAddress());
  }
  RfSettings(const RfSettings& rhs) :
    SettingsBase(rhs),
    m_frequency(this, rhs.m_frequency),
    m_frequencyStep(this, rhs.m_frequencyStep),
    m_offset(this, rhs.m_offset),
    m_offsetStep(this, rhs.m_offsetStep),
    m_gain(this, rhs.m_gain),
    m_gainStep(this, rhs.m_gainStep)
  {
    m_frequency.setStepValueAddress(m_frequencyStep.getValueAddress());
    m_offset.setStepValueAddress(m_offsetStep.getValueAddress());
  }
  // RfSettings(const RfSettings& rhs) = default;
  ~RfSettings() override = default;

  RfSettings& operator=(const RfSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      m_frequency = rhs.m_frequency;
      m_frequencyStep = rhs.m_frequencyStep;
      m_offset = rhs.m_offset;
      m_offsetStep = rhs.m_offsetStep;
      m_gain = rhs.m_gain;
      m_gainStep = rhs.m_gainStep;
    }
    return *this;
  }

  [[nodiscard]] uint32_t getFrequency() const { return m_frequency(); }
  [[nodiscard]] int32_t getFrequencyStep() const { return m_frequencyStep(); }
  [[nodiscard]] int32_t getOffset() const { return m_offset(); }
  [[nodiscard]] int32_t getOffsetStep() const { return m_offsetStep(); }
  [[nodiscard]] float getGain() const { return m_gain(); }
  [[nodiscard]] float getGainStep() const { return m_gainStep(); }

  void setFrequency(uint32_t frequency) { m_frequency(frequency); }
  void setFrequencyStep(int32_t frequencyStep) { m_frequencyStep(frequencyStep); }
  void setOffset(int32_t offset) { m_offset(offset); }
  void setOffsetStep(int32_t offsetStep) { m_offsetStep(offsetStep); }
  void setGain(float gain) { m_gain(gain); }
  void setGainStep(float gainStep) { m_gainStep(gainStep); }

  // void clearChanged() override
  // {
  //   qDebug() << "RfSettings::clearChanged called";
  //   SettingsBase::clearChanged();
  // }

  // void setAllChanged() override
  // {
  //   qDebug() << "RfSettings::setAllChanged called";
  //   SettingsBase::setAllChanged();
  // }

  void copyFrequencies(const RfSettings& rhs)
  {
    m_frequency.merge(rhs.m_frequency);
    m_offset.merge(rhs.m_offset);
    m_frequencyStep.merge(rhs.m_frequencyStep);
    m_offsetStep.merge(rhs.m_offsetStep);
  }

  bool setBand(const Band& band)
  {
    if (band.isValid()) {
      uint64_t freqPlusOffset = m_frequency() + m_offset();
      if (!band.containsFrequency(freqPlusOffset)) {
        m_frequency(band.getLandingFrequency());
        m_offset(0);
        return true;
      }
    }
    return false;
  }

  bool merge(const RfSettings& settings)
  {
    return m_frequency.merge(settings.m_frequency)
      | m_frequencyStep.merge(settings.m_frequencyStep)
      | m_offset.merge(settings.m_offset)
      | m_offsetStep.merge(settings.m_offsetStep)
      | m_gain.merge(settings.m_gain)
      | m_gainStep.merge(settings.m_gainStep);
  }

  bool applyUpdate(const SettingUpdate& update, int startIndex) override
  {
    const auto& features = update.getPath().getFeatures();
    if (startIndex >= features.size()) {
      throw SettingsException("Invalid setting path");
    }
    uint32_t feature = features[startIndex];
    const auto& val = update.getValue();

    switch (feature) {
    case FREQUENCY: return m_frequency.apply(update);
    case FREQUENCY_STEP: return m_frequencyStep.apply(val);
    case OFFSET: return m_offset.apply(update);
    case OFFSET_STEP: return m_offsetStep.apply(val);
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
    return resolvePathForRegisteredSetting<RfSettings>(featureStrings, out, startIndex);
  }

protected:
  Setting<uint32_t, FREQUENCY, RfSettings, int32_t> m_frequency;
  Setting<int32_t, FREQUENCY_STEP, RfSettings> m_frequencyStep;
  Setting<int32_t, OFFSET, RfSettings, int32_t> m_offset;
  Setting<int32_t, OFFSET_STEP, RfSettings> m_offsetStep;
  Setting<float, GAIN, RfSettings> m_gain;
  Setting<float, GAIN_STEP, RfSettings> m_gainStep;
};
