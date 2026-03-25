//
// Created by murray on 27/1/26.
//

#include "RadioSettings.h"

#include "core/util/StringUtils.h"


BandSettings*
RadioSettings::getFocusBandSettings()
{
  BandSettings* pBandSettings = m_activeBandSettings.getFocusBandSettings();
  if (pBandSettings == nullptr) {
    throw SettingsException("No band selected");
  }
  return pBandSettings;
}

const BandSettings*
RadioSettings::getFocusBandSettings() const
{
 const BandSettings* pBandSettings = m_activeBandSettings.getFocusBandSettings();
  if (pBandSettings == nullptr) {
    throw SettingsException("No band selected");
  }
  return pBandSettings;
}

// void
// RadioSettings::setCentreFrequencyDeltas(int32_t fine, int32_t coarse)
// {
//   m_activeBandSettings.setCentreFrequencyDeltas(fine, coarse);
// }

// void
// RadioSettings::applyRfSettings(const RfSettings& settings, bool onlyChanged)
// {
//   m_activeBandSettings.applyRfSettings(settings, onlyChanged);
//   m_changed |= BAND;
// }
//
// void
// RadioSettings::applyIfSettings(const IfSettings& settings)
// {
//   m_activeBandSettings.applyIfSettings(settings);
//   m_changed |= BAND;
// }

RxPipelineSettings*
RadioSettings::getFocusPipeline()
{
  BandSettings* pBandSettings = getFocusBandSettings();
  return pBandSettings->getFocusPipeline();
}

const RxPipelineSettings*
RadioSettings::getFocusPipeline() const
{
  const BandSettings* pBandSettings = getFocusBandSettings();
  return pBandSettings->getFocusPipeline();
}

TxPipelineSettings*
RadioSettings::getTxPipelineSettings()
{
  BandSettings* pBandSettings = getFocusBandSettings();
  return pBandSettings->getTxPipeline();
}

const TxPipelineSettings*
RadioSettings::getTxPipelineSettings() const
{
  const BandSettings* pBandSettings = getFocusBandSettings();
  return pBandSettings->getTxPipeline();
}

const Mode*
RadioSettings::getFocusRxPipelineMode() const
{
  const BandSettings* pBandSettings = getFocusBandSettings();
  return pBandSettings->getFocusRxMode();
}

bool
RadioSettings::applyUpdate(SettingUpdate& update, BandSelector& bandSelector)
{
  if (update.isExhausted()) {
    throw SettingsException("Invalid setting path");
  }
  uint32_t feature = update.getCurrentFeature();
  if (feature == BAND) {
    if (m_activeBandSettings.applyUpdate(update.stepNextFeature(), bandSelector)) {
      m_changed |= BAND;
      return true;
    }
    return false;

  }
  return applyUpdate(update);
}

bool
RadioSettings::applyUpdate(SettingUpdate& update)
{
  uint32_t feature = update.getCurrentFeature();
  const auto& val = update.getValue();

  switch (feature) {
  case PTT:
    return m_ptt.apply(val);
  case TX:
    update.stepNextFeature();
    if (m_txSettings.applyUpdate(update)) {
      m_changed |= TX;
      return true;
    }
    return false;
  case RX:
    update.stepNextFeature();
    if (m_rxSettings.applyUpdate(update)) {
      m_changed |= RX;
      return true;
    }
    return false;
  default:
    return false;
  }
}

SettingUpdatePath
RadioSettings::getSettingUpdatePath(const std::string& strDottedFeatures)
{
  std::string featuresLower = StringUtils::toLowerCase(strDottedFeatures);
  std::vector<std::string> featureStrings = StringUtils::split(featuresLower, '.');
  std::vector<uint32_t> features;
  if (!getFeaturePath(featureStrings, features)) {
    throw SettingsException("Unknown RadioSettings setting: " + strDottedFeatures);
  }
  return SettingUpdatePath(features);
}

bool
RadioSettings::getFeaturePath(
  const std::vector<std::string>& featureStrings,
  std::vector<uint32_t>& featuresOut,
  size_t startIndex
  )
{
  if (startIndex >= featureStrings.size()) {
    throw SettingsException("Invalid feature path");
  }

  if (resolvePathForRegisteredSetting<RadioSettings>(featureStrings, featuresOut, startIndex)) {
    return true;
  }

  const std::string& key = featureStrings[startIndex];
  if (key == "band") {
    featuresOut.push_back(BAND);
    return ActiveBandSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
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
