//
// Created by murray on 14/04/23.
//

#pragma once

#include <cstdint>

#include "base/SettingsBase.h"

class IfSettings : public SettingsBase {
public:
  enum Features
  {
    NONE = 0,
    BANDWIDTH = 0x01,
    GAIN = 0x02,
    ALL = static_cast<uint32_t>(~0U)
  };
  IfSettings() :
    m_bandwidth(this, "bandwidth", 0),
    m_gain(this, "gain", 0.0),
  {
  }
  IfSettings(const IfSettings& rhs) :
    SettingsBase(rhs),
    m_bandwidth(this, rhs.m_bandwidth),
    m_gain(this, rhs.m_gain),
  {
  }
  ~IfSettings() override = default;

  IfSettings& operator=(const IfSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      m_bandwidth = rhs.m_bandwidth;
      m_gain = rhs.m_gain;
     }
    return *this;
  }

  [[nodiscard]] uint32_t getBandwidth() const { return m_bandwidth(); }
  void setBandwidth(uint32_t bandwidth) { m_bandwidth(bandwidth); }

  [[nodiscard]] float getGain() const { return m_gain(); }
  void setGain(float gain) { m_gain(gain); }

  bool merge(const IfSettings& rhs)
  {
    return m_bandwidth.merge(rhs.m_bandwidth) | m_gain.merge(rhs.m_gain);
  }

  bool applyUpdate(SettingUpdate& update) override
  {
    if (update.isExhausted()) {
      throw SettingsException("Invalid setting path");
    }
    uint32_t feature = update.getCurrentFeature();
    const auto& val = update.getValue();

    switch (feature) {
    case BANDWIDTH: return m_bandwidth.apply(val);
    case GAIN:  return m_gain.apply(val);
    default: return false;
    }
  }

  static bool getFeaturePath(
    const std::vector<std::string>& featureStrings,
    std::vector<uint32_t>& out,
    size_t startIndex
    )
  {
    return resolvePathForRegisteredSetting<IfSettings>(featureStrings, out, startIndex);
  }

protected:
  Setting<uint32_t,BANDWIDTH, IfSettings> m_bandwidth;
  Setting<float, GAIN, IfSettings> m_gain;
};

