//
// Created by murray on 14/04/23.
//

#pragma once


#include <cstdint>

#include "Bands.h"
#include "SettingsBase.h"
#include "SettingsException.h"
#include <QDebug>

#include "SteppableSetting.h"

class RfSettings : public SettingsBase
{
public:
  enum Features
  {
    NONE = 0,
    CENTRE_FREQUENCY = 0x01,
    OFFSET = 0x02,
    GAIN = 0x04,
    ALL = static_cast<uint32_t>(~0U)
  };
  RfSettings() :
    SettingsBase()
    ,m_centreFrequency(this, "centre-frequency")
    ,m_offset(this, "offset")
    ,m_gain(this, "gain")
  {
  }
  RfSettings(const RfSettings& rhs) :
    SettingsBase(rhs)
    ,m_centreFrequency(this, "centre-frequency")
    ,m_offset(this, "offset")
    ,m_gain(this, "gain")
  {
    merge(rhs);
  }
  ~RfSettings() override = default;

  RfSettings& operator=(const RfSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      m_centreFrequency = rhs.m_centreFrequency;
      m_offset = rhs.m_offset;
      m_gain = rhs.m_gain;
    }
    return *this;
  }

  [[nodiscard]] uint32_t getFrequency() const { return m_centreFrequency.getValue(); }
  [[nodiscard]] int32_t getOffset() const { return m_offset.getValue(); }
  [[nodiscard]] float getGain() const { return m_gain.getValue(); }

  void setCentreFrequency(uint32_t frequency)
  {
    m_centreFrequency.setValue(frequency);
    m_changed |= CENTRE_FREQUENCY;
  }
  void setCentreFrequencyCoarseStep(int32_t step)
  {
    m_centreFrequency.setCoarseStep(step);
    m_changed |= CENTRE_FREQUENCY;
  }
  void setCentreFrequencyFineStep(int32_t step)
  {
    m_centreFrequency.setFineStep(step);
    m_changed |= CENTRE_FREQUENCY;
  }

  void setOffset(int32_t offset)
  {
    m_offset.setValue(offset);
    m_changed |= OFFSET;
  }
  void setOffsetCoarseStep(int32_t step)
  {
    m_offset.setCoarseStep(step);
    m_changed |= OFFSET;
  }
  void setOffsetFineStep(int32_t step)
  {
    m_offset.setFineStep(step);
    m_changed |= OFFSET;
  }

  void setGain(float gain)
  {
    m_gain.setValue(gain);
    m_changed |= GAIN;
  }
  void setGainCoarseStep(float step)
  {
    m_gain.setCoarseStep(step);
    m_changed |= GAIN;
  }
  void setGainFineStep(float step)
  {
    m_gain.setCoarseStep(step);
    m_changed |= GAIN;
  }

  void copyFrequencies(const RfSettings& rhs)
  {
    m_centreFrequency.merge(rhs.m_centreFrequency);
    m_offset.merge(rhs.m_offset);
  }

  bool applyBandDefaults(const Band& band)
  {
    if (band.isValid()) {
      uint64_t freqPlusOffset = m_centreFrequency.getValue() + m_offset.getValue();
      if (!band.containsFrequency(freqPlusOffset)) {
        // Adjusting 10kHz gets the landing frequency off any centre spike (in case of poor IQ correction)
        m_centreFrequency.setValue(band.getLandingFrequency() - 10000);
        // Note: The centre frequency steps need to ba a function of the sample rate (we don't want gaps
        // in what we see of the band when we step the centre frequency.
        m_offset.setValue(10000);
        m_offset.setCoarseStep(band.getDefaultCoarseStep());
        m_offset.setFineStep(band.getDefaultFineStep());
        return true;
      }
    }
    return false;
  }

  void setAllChanged() override
  {
    SettingsBase::setAllChanged();
    m_centreFrequency.setAllChanged();
    m_offset.setAllChanged();
    m_gain.setAllChanged();
  }

  bool merge(const RfSettings& settings, bool onlyChanged = false)
  {
    bool changed = false;
    if (m_centreFrequency.merge(settings.m_centreFrequency, onlyChanged) ) {
      m_changed |= CENTRE_FREQUENCY;
      changed = true;
    }
    if (m_offset.merge(settings.m_offset, onlyChanged)) {
      m_changed |= OFFSET;
      changed = true;
    }
    if (m_gain.merge(settings.m_gain, onlyChanged)) {
      m_changed |= GAIN;
      changed = true;
    }
    return changed;
  }

  bool applyUpdate(SettingUpdate& update) override
  {
    if (update.isExhausted()) {
      throw SettingsException("Invalid setting path");
    }
    uint32_t feature = update.getCurrentFeature();
    const auto& val = update.getValue();

    switch (feature) {
    case CENTRE_FREQUENCY: return m_centreFrequency.applyUpdate(update.stepNextFeature());
    case OFFSET: return m_offset.applyUpdate(update.stepNextFeature());
    case GAIN: return m_gain.applyUpdate(update.stepNextFeature());
    default:
      return false;
    }
  }

  static bool getFeaturePath(
    const std::vector<std::string>& featureStrings,
    std::vector<uint32_t>& out,
    size_t startIndex
    )
  {
    if (startIndex >= featureStrings.size()) {
      throw SettingsException("Invalid feature path");
    }
    if (resolvePathForRegisteredSetting<RfSettings>(featureStrings, out, startIndex)) {
      if (CentreFrequencyType::getFeaturePath(featureStrings, out, startIndex + 1)) {
        return true;
      }
      if (OffsetType::getFeaturePath(featureStrings, out, startIndex + 1)) {
        return true;
      }
      if (GainType::getFeaturePath(featureStrings, out, startIndex + 1)) {
        return true;
      }
    }
    return false;
  }

protected:
  using CentreFrequencyType = SteppableSetting<uint32_t, CENTRE_FREQUENCY, RfSettings, int32_t>;
  using OffsetType = SteppableSetting<int32_t, OFFSET, RfSettings>;
  using GainType = SteppableSetting<float, GAIN, RfSettings>;

  CentreFrequencyType m_centreFrequency;
  OffsetType m_offset;
  GainType m_gain;
};
