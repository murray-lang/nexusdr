//
// Created by murray on 27/1/26.
//

#include "RadioSettings.h"

BandSelector RadioSettings::m_bandSelector;

BandSettings*
RadioSettings::getBandSettings(const std::string& bandName)
{
  return m_bandSelector.getBandSettings(bandName);
}

BandSettings*
RadioSettings::getFocusBandSettings()
{
  BandSettings* pBandSettings = m_bandSelector.getFocusBandSettings();
  if (pBandSettings == nullptr) {
    throw SettingsException("No band selected");
  }
  return pBandSettings;
}

void
RadioSettings::setCentreFrequencyDeltas(int32_t fine, int32_t coarse)
{
  m_bandSelector.setCentreFrequencyDeltas(fine, coarse);
}

void
RadioSettings::applyRfSettings(const RfSettings& settings, bool onlyChanged)
{
  m_bandSelector.applyRfSettings(settings, onlyChanged);
  m_changed |= BAND;
}

void
RadioSettings::applyIfSettings(const IfSettings& settings)
{
  m_bandSelector.applyIfSettings(settings);
  m_changed |= BAND;
}

RxPipelineSettings*
RadioSettings::getFocusRxPipelineSettings()
{
  BandSettings* pBandSettings = getFocusBandSettings();
  return pBandSettings->getFocusRxPipelineSettings();
}

const RxPipelineSettings*
RadioSettings::getFocusRxPipelineSettings() const
{
  const BandSettings* pBandSettings = getFocusBandSettings();
  return pBandSettings->getFocusRxPipelineSettings();
}

TxPipelineSettings*
RadioSettings::getTxPipelineSettings()
{
  BandSettings* pBandSettings = getFocusBandSettings();
  return pBandSettings->getTxPipelineSettings();
}

const TxPipelineSettings*
RadioSettings::getTxPipelineSettings() const
{
  const BandSettings* pBandSettings = getFocusBandSettings();
  return pBandSettings->getTxPipelineSettings();
}

const Mode&
RadioSettings::getFocusRxPipelineMode() const
{
  BandSettings* pBandSettings = getFocusBandSettings();
  return pBandSettings->getFocusRxPipelineMode();
}

bool
RadioSettings::applyUpdate(SettingUpdate& update)
{
  if (update.isExhausted()) {
    throw SettingsException("Invalid setting path");
  }
  uint32_t feature = update.getCurrentFeature();
  const auto& val = update.getValue();

  switch (feature) {
  case PTT:
    return m_ptt.apply(val);
  case BAND:
    if (m_bandSelector.applyUpdate(update.stepNextFeature())) {
      m_changed |= BAND;
      return true;
    }
    return false;
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
    return BandSelector::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
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