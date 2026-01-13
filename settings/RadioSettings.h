//
// Created by murray on 15/04/23.
//

#pragma once


#include "ModeSettings.h"
#include "ReceiverSettings.h"
#include "TransmitterSettings.h"
#include "SettingUpdatePath.h"
#include <cstdint>
#include <string>
#include <vector>

#include "Band.h"
#include "Bands.h"
#include "BandSettings.h"
#include "SettingsException.h"
#include "../util/StringUtils.h"


class RadioSettings : public SettingsBase {
public:
  enum Features
  {
    NONE = 0,
    PTT = 0x01,
    PIPELINE = 0x02,
    TX = 0x04,
    RX = 0x08,
    BAND = 0x10,
    ALL = static_cast<uint32_t>(~0U)
  };

  RadioSettings() : m_ptt(this, "ptt", false), m_bandName(this, "band", "")
  {
    RadioSettings::setAllChanged();
  };
  // RadioSettings(const RadioSettings& rhs) = default;

  RadioSettings(const RadioSettings& rhs) : 
  SettingsBase(rhs),
  m_ptt(rhs.m_ptt),
  m_bandName(rhs.m_bandName),
  m_rxSettings(rhs.m_rxSettings),
  m_txSettings(rhs.m_txSettings)
  {
  }
  ~RadioSettings() override = default;

  RadioSettings& operator=(const RadioSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      m_bandName = rhs.m_bandName;
      m_rxSettings = rhs.m_rxSettings;
      m_txSettings = rhs.m_txSettings;
    }
    return *this;
  }

  [[nodiscard]] bool getPtt() const { return m_ptt(); }
  [[nodiscard]] const std::string& getBandName() const { return m_bandName(); }
  [[nodiscard]] const ReceiverSettings& getRxSettings() const { return m_rxSettings; }
  [[nodiscard]] const TransmitterSettings& getTxSettings() const { return m_txSettings; }

  bool applyUpdate(const SettingUpdate& update, int startIndex) override
  {
    const auto& features = update.getPath().getFeatures();
    if (startIndex >= features.size()) {
      throw SettingsException("Invalid setting path");
    }

    uint32_t feature = features[startIndex];
    const auto& val = update.getValue();

    switch (feature) {
    case PTT:
      return m_ptt.apply(val);
    case BAND:
      if (m_bandName.apply(val)) {
        m_changed |= PIPELINE; // Side effect: Band change affects pipeline
        return true;
      }
      return false;
    case TX:
      if (m_txSettings.applyUpdate(update, startIndex + 1)) {
        m_changed |= TX;
        return true;
      }
      return false;
    case RX:
      if (m_rxSettings.applyUpdate(update, startIndex + 1)) {
        m_changed |= RX;
        return true;
      }
      return false;
    default:
      return false;
    }
  }

  void clearChanged() override
  {
    SettingsBase::clearChanged();
    m_rxSettings.clearChanged();
    m_txSettings.clearChanged();
  }

  void setAllChanged() override
  {
    SettingsBase::setAllChanged();
    m_rxSettings.setAllChanged();
    m_txSettings.setAllChanged();
    // Not PTT! That being set will short-circuit all other changes
    m_changed &= ~PTT;
  }

  void markPipelineChanged() { m_changed |= PIPELINE; }

  static SettingUpdatePath getSettingUpdatePath(const std::string& strDottedFeatures)
  {
    std::string featuresLower = StringUtils::toLowerCase(strDottedFeatures);
    std::vector<std::string> featureStrings = StringUtils::split(featuresLower, '.');
    std::vector<uint32_t> features;
    if (!getFeaturePath(featureStrings, features)) {
      throw SettingsException("Unknown RadioSettings setting: " + strDottedFeatures);
    }
    return SettingUpdatePath(features);
  }

  static bool getFeaturePath(
    const std::vector<std::string>& featureStrings,
    std::vector<uint32_t>& featuresOut,
    size_t startIndex = 0
    )
  {
    if (startIndex >= featureStrings.size()) {
      throw SettingsException("Invalid feature path");
    }

    if (resolvePathForRegisteredSetting<RadioSettings>(featureStrings, featuresOut, startIndex)) {
      return true;
    }

    const std::string& key = featureStrings[startIndex];
    if (key == "pipeline") {
      // Note that BandSettings contains separate pipeline settings for each band. This is why there is
      // currently a confusing mismatch between pipeline settings and BandSettings.
      // Also, BandSettings is not a member of RadioSettings for efficiency reasons. However,
      // pipeline settings paths are managed here for convenience.
      featuresOut.push_back(PIPELINE);
      return BandSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
    }
    if (key == "rx") {
      featuresOut.push_back(RX);
      return ReceiverSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
    }
    if (key == "tx") {
      featuresOut.push_back(TX);
      return TransmitterSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
    }
    return false;
  }
protected:
  Setting<bool, PTT, RadioSettings> m_ptt;
  Setting<std::string, BAND, RadioSettings> m_bandName;
  ReceiverSettings m_rxSettings;
  TransmitterSettings m_txSettings;
};
