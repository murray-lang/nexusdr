//
// Created by murray on 14/04/23.
//

#pragma once

#include "IqCorrectionSettings.h"
#include "MicSettings.h"
#include "RfSettings.h"
#include "TestSettings.h"
#include "ModeSettings.h"

class TransmitterSettings : public SettingsBase {
public:
  enum Features
  {
    NONE = 0,
    // MODE = 0x01,
    // RF = 0x02,
    CORRECTION = 0x04,
    MIC = 0x08,
    TEST = 0x10,
    // BAND = 0x20
    ALL = static_cast<uint32_t>(~0U)
  };
  TransmitterSettings() = default;
  TransmitterSettings(const TransmitterSettings& rhs) = default;
  ~TransmitterSettings() override = default;

  TransmitterSettings& operator=(const TransmitterSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      // rfSettings = rhs.rfSettings;
      m_correctionSettings = rhs.m_correctionSettings;
      m_micSettings = rhs.m_micSettings;
      m_testSettings = rhs.m_testSettings;
      // mode = rhs.mode;
      // band = rhs.band;
    }
    return *this;
  }

  bool merge
  (const TransmitterSettings& settings)
  {
    return m_correctionSettings.merge(settings.m_correctionSettings)
    | m_micSettings.merge(settings.m_micSettings)
    | m_testSettings.merge(settings.m_testSettings);
  }

  IqCorrectionSettings& getCorrectionSettings() { return m_correctionSettings; }
  MicSettings& getMicSettings() { return m_micSettings; }
  TestSettings& getTestSettings() { return m_testSettings; }

  [[nodiscard]] const IqCorrectionSettings& getCorrectionSettings() const { return m_correctionSettings; }
  [[nodiscard]] const MicSettings& getMicSettings() const { return m_micSettings; }
  [[nodiscard]] const TestSettings& getTestSettings() const { return m_testSettings; }

  bool applyUpdate(SettingUpdate& update) override
  {
    if (update.isExhausted()) {
      throw SettingsException("Invalid setting path");
    }
    uint32_t feature = update.getCurrentFeature();
    const auto& val = update.getValue();
    bool settingChange = false;
    if (feature == CORRECTION) {
      update.stepNextFeature();
      settingChange = m_correctionSettings.applyUpdate(update);
    } else if (feature == MIC) {
      update.stepNextFeature();
      settingChange = m_micSettings.applyUpdate(update);
    } else if (feature == TEST) {
      update.stepNextFeature();
      settingChange = m_testSettings.applyUpdate(update);
    }
    if (settingChange) {
      m_changed |= feature;
    }
    return settingChange;
  }

  void clearChanged() override
  {
    SettingsBase::clearChanged();
    m_correctionSettings.clearChanged();
    m_micSettings.clearChanged();
    m_testSettings.clearChanged();
  }

  void markAllChanged() override
  {
    SettingsBase::markAllChanged();
    m_correctionSettings.markAllChanged();
    m_micSettings.markAllChanged();
    m_testSettings.markAllChanged();
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
    // if (featureStrings[startIndex] == "rf") {
    //   featuresOut.push_back(RF);
    //   if (startIndex + 1 < featureStrings.size()) {
    //     RfSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
    //   }
    // } else
    if (featureStrings[startIndex] == "correction") {
      featuresOut.push_back(CORRECTION);
      return IqCorrectionSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
    }
    if (featureStrings[startIndex] == "mic") {
      featuresOut.push_back(MIC);
      return MicSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
    }
    if (featureStrings[startIndex] == "test") {
      featuresOut.push_back(TEST);
      return TestSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
    }
    return false;
  }
protected:
  IqCorrectionSettings m_correctionSettings;
  MicSettings m_micSettings;
  TestSettings m_testSettings;
};
