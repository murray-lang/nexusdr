//
// Created by murray on 14/04/23.
//

#pragma once


#include "bands/Band.h"
#include "RfSettings.h"
#include "IfSettings.h"
#include "IqCorrectionSettings.h"
#include "core/SettingsException.h"
#include "Mode.h"
#include "bands/Band.h"
#include "bands/Bands.h"
#include "ModeSettings.h"
#include <map>

class ReceiverSettings : public SettingsBase {
public:
  enum Features
  {
    NONE = 0,
    CORRECTION = 0x01,
    ALL = static_cast<uint32_t>(~0U)
  };

  explicit ReceiverSettings(/*const Bands& bands, const ModeSettings& modeSettings*/) :
    SettingsBase() //, mode(), modeSettings(modeSettings), band(), bands(bands), rfSettings(), ifSettings()
  {};
  ReceiverSettings(const ReceiverSettings& rhs) = default;

  ~ReceiverSettings() override = default;

  ReceiverSettings& operator=(const ReceiverSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      m_correctionSettings = rhs.m_correctionSettings;
    }
    return *this;
  }

  IqCorrectionSettings& getCorrectionSettings()
  {
    return m_correctionSettings;
  }

  const IqCorrectionSettings& getCorrectionSettings() const
  {
    return m_correctionSettings;
  }

  bool merge(const ReceiverSettings& settings)
  {
    if (m_correctionSettings.merge(settings.m_correctionSettings)) {
      m_changed |= CORRECTION;
      return true;
    }
    return false;
  }

  bool applyUpdate(SettingUpdate& update) override
  {
    if (update.isExhausted()) {
      throw SettingsException("Invalid setting path");
    }
    uint32_t feature = update.getCurrentFeature();


    bool settingChange = false;
    if (feature == CORRECTION) {
      update.stepNextFeature();
      settingChange = m_correctionSettings.applyUpdate(update);
    }
    if (settingChange) {
      m_changed |= feature;
    }
    return false;
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
    const std::string& key = featureStrings[startIndex];
    if (key == "correction") {
      featuresOut.push_back(CORRECTION);
      return IqCorrectionSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
    }
    return false;
  }
protected:
  IqCorrectionSettings m_correctionSettings;
};

