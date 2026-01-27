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
    CENTER_FREQUENCY = 0x01,
    VFO = 0x02,
    GAIN = 0x04,
    ALL = static_cast<uint32_t>(~0U)
  };
  RfSettings() :
    SettingsBase()
    ,m_centreFrequency(this, "center-frequency", 0, 10000, 50000)
    ,m_vfo(this, "vfo", 0, 100, 1000)
    ,m_gain(this, "gain", 0.0, 0.1, 1.0)
    ,m_maxNegativeVfoOffset(0)
    ,m_maxPositiveVfoOffset(0)
  {
  }
  RfSettings(const RfSettings& rhs) :
    SettingsBase(rhs)
    ,m_centreFrequency(this, "center-frequency", 0, 10000, 50000)
    ,m_vfo(this, "vfo", 0, 100, 1000)
    ,m_gain(this, "gain", 0.0, 0.1, 1.0)
    ,m_maxNegativeVfoOffset(rhs.m_maxNegativeVfoOffset)
    ,m_maxPositiveVfoOffset(rhs.m_maxPositiveVfoOffset)
  {
    merge(rhs);
  }
  ~RfSettings() override = default;

  RfSettings& operator=(const RfSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      m_centreFrequency = rhs.m_centreFrequency;
      m_vfo = rhs.m_vfo;
      m_gain = rhs.m_gain;
      m_maxPositiveVfoOffset = rhs.m_maxPositiveVfoOffset;
      m_maxNegativeVfoOffset = rhs.m_maxNegativeVfoOffset;
    }
    return *this;
  }

  [[nodiscard]] int64_t getCentreFrequency() const { return m_centreFrequency.getValue(); }
  [[nodiscard]] int64_t getVfo() const { return m_vfo.getValue(); }
  [[nodiscard]] float getGain() const { return m_gain.getValue(); }

  void setMaximumVfoOffsets(int32_t negativeOffset, int32_t positiveOffset)
  {
    m_maxNegativeVfoOffset = negativeOffset;
    m_maxPositiveVfoOffset = positiveOffset;
  }

  bool applyMaximumVfoOffsets(int32_t negativeOffset, int32_t positiveOffset)
  {
    setMaximumVfoOffsets(negativeOffset, positiveOffset);
    bool changed = false;
    int64_t centreFrequency = m_centreFrequency.getValue();
    int64_t offset = m_vfo.getValue() - centreFrequency;
    if (offset > m_maxPositiveVfoOffset) {
      m_vfo.setValue(centreFrequency + m_maxPositiveVfoOffset);
      m_changed |= VFO;
      changed = true;
    } else if (offset < m_maxNegativeVfoOffset) {
      m_vfo.setValue(centreFrequency + m_maxNegativeVfoOffset);
      m_changed |= VFO;
      changed = true;
    }
    return changed;
  }

  void setCentreFrequency(uint32_t frequency)
  {
    m_centreFrequency.setValue(frequency);
    m_changed |= CENTER_FREQUENCY;
  }

  void setCentreFrequencyDeltas(int32_t fine, int32_t coarse)
  {
    m_centreFrequency.setCoarseDelta(fine);
    m_centreFrequency.setCoarseDelta(coarse);
    m_changed |= CENTER_FREQUENCY;
  }
  void setCentreFrequencyCoarseDelta(int32_t delta)
  {
    m_centreFrequency.setCoarseDelta(delta);
    m_changed |= CENTER_FREQUENCY;
  }
  void setCentreFrequencyFineDelta(int32_t delta)
  {
    m_centreFrequency.setFineDelta(delta);
    m_changed |= CENTER_FREQUENCY;
  }

  void setVfo(int32_t offset)
  {
    m_vfo.setValue(offset);
    m_changed |= VFO;
  }
  void setVfoCoarseDelta(int32_t step)
  {
    m_vfo.setCoarseDelta(step);
    m_changed |= VFO;
  }
  void setVfoFineDelta(int32_t step)
  {
    m_vfo.setFineDelta(step);
    m_changed |= VFO;
  }

  void setGain(float gain)
  {
    m_gain.setValue(gain);
    m_changed |= GAIN;
  }
  void setGainCoarseStep(float step)
  {
    m_gain.setCoarseDelta(step);
    m_changed |= GAIN;
  }
  void setGainFineStep(float step)
  {
    m_gain.setCoarseDelta(step);
    m_changed |= GAIN;
  }

  void copyFrequencies(const RfSettings& rhs)
  {
    m_centreFrequency.merge(rhs.m_centreFrequency);
    m_vfo.merge(rhs.m_vfo);
  }

  bool applyBandDefaults(const Band& band)
  {
    if (band.isValid()) {
      uint64_t freqPlusOffset = m_centreFrequency.getValue() + m_vfo.getValue();
      if (!band.containsFrequency(freqPlusOffset)) {
        // Adjusting 10kHz gets the landing frequency off any centre spike (in case of poor IQ correction)
        m_centreFrequency.setValue(band.getLandingFrequency() - 10000);
        // Note: The centre frequency steps need to ba a function of the sample rate (we don't want gaps
        // in what we see of the band when we step the centre frequency.
        m_vfo.setValue(band.getLandingFrequency());
        m_vfo.setCoarseDelta(band.getDefaultCoarseStep());
        m_vfo.setFineDelta(band.getDefaultFineStep());
        return true;
      }
    }
    return false;
  }

  void setAllChanged() override
  {
    SettingsBase::setAllChanged();
    m_centreFrequency.setAllChanged();
    m_vfo.setAllChanged();
    m_gain.setAllChanged();
  }

  bool merge(const RfSettings& settings, bool onlyChanged = false)
  {
    bool changed = false;
    if (m_centreFrequency.merge(settings.m_centreFrequency, onlyChanged) ) {
      m_changed |= CENTER_FREQUENCY;
      changed = true;
    }
    if (m_vfo.merge(settings.m_vfo, onlyChanged)) {
      m_changed |= VFO;
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

    bool frequencyChange = false;
    switch (feature) {
    case CENTER_FREQUENCY:
      if (m_centreFrequency.applyUpdate(update.stepNextFeature())) {
        frequencyChange = true;
      }
      break;
    case VFO:
      if (m_vfo.applyUpdate(update.stepNextFeature())) {
        frequencyChange = true;
      }
      break;
    case GAIN: return m_gain.applyUpdate(update);
    default: ;
    }
    // Clamp frequencies that are out of Nyquist
    if (frequencyChange) {
      int64_t centreFrequency = m_centreFrequency.getValue();
      int64_t offset = m_vfo.getValue() - centreFrequency;
      if (offset > m_maxPositiveVfoOffset) {
        m_vfo.setValue(centreFrequency + m_maxPositiveVfoOffset);
      } else if (offset < m_maxNegativeVfoOffset) {
        m_vfo.setValue(centreFrequency + m_maxNegativeVfoOffset);
      }
    }
    return frequencyChange;
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
      if (VfoType::getFeaturePath(featureStrings, out, startIndex + 1)) {
        return true;
      }
      if (GainType::getFeaturePath(featureStrings, out, startIndex + 1)) {
        return true;
      }
    }
    return false;
  }

protected:
  using CentreFrequencyType = SteppableSetting<int64_t, CENTER_FREQUENCY, RfSettings, int64_t>;
  using VfoType = SteppableSetting<int64_t, VFO, RfSettings>;
  using GainType = SteppableSetting<float, GAIN, RfSettings>;

  CentreFrequencyType m_centreFrequency;
  VfoType m_vfo;
  GainType m_gain;

  // These could differ due to the modulation mode
  int32_t m_maxNegativeVfoOffset;
  int32_t m_maxPositiveVfoOffset;
};
