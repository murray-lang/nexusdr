//
// Created by murray on 27/1/26.
//

#include "RadioSettings.h"

#include "core/util/StringUtils.h"

#ifdef USE_ETL
#include "etl/string_utilities.h"
#endif


BandSettings*
RadioSettings::getFocusBandSettings()
{
  BandSettings* pBandSettings = m_activeBandSettings.getFocusBandSettings();
  return pBandSettings;
}

const BandSettings*
RadioSettings::getFocusBandSettings() const
{
 const BandSettings* pBandSettings = m_activeBandSettings.getFocusBandSettings();
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
  if (pBandSettings != nullptr) {
    return pBandSettings->getFocusPipeline();
  }
  return nullptr;
}

const RxPipelineSettings*
RadioSettings::getFocusPipeline() const
{
  const BandSettings* pBandSettings = getFocusBandSettings();
  if (pBandSettings != nullptr) {
    return pBandSettings->getFocusPipeline();
  }
  return nullptr;
}

TxPipelineSettings*
RadioSettings::getTxPipelineSettings()
{
  BandSettings* pBandSettings = getFocusBandSettings();
  if (pBandSettings != nullptr) {
    return pBandSettings->getTxPipeline();
  }
  return nullptr;
}

const TxPipelineSettings*
RadioSettings::getTxPipelineSettings() const
{
  const BandSettings* pBandSettings = getFocusBandSettings();
  if (pBandSettings != nullptr) {
    return pBandSettings->getTxPipeline();
  }
  return nullptr;
}

const Mode*
RadioSettings::getFocusRxPipelineMode() const
{
  const BandSettings* pBandSettings = getFocusBandSettings();
  if (pBandSettings != nullptr) {
    return pBandSettings->getFocusRxMode();
  }
  return nullptr;
}

bool
RadioSettings::applyUpdate(SettingUpdate& update, BandSelector& bandSelector)
{
  if (update.isExhausted()) {
    return false;
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

ResultCode
RadioSettings::getSettingUpdatePath(const SettingPathString& strDottedFeatures, SettingUpdatePath& path)
{
#ifdef USE_ETL
  SettingPathString featuresLower = strDottedFeatures;
  etl::to_lower_case(featuresLower);
  etl::vector<etl::string_view, MAX_SETTING_DEPTH> featureStringsView;
  etl::get_token_list(featuresLower, featureStringsView, ".", true, MAX_SETTING_DEPTH);
  FeatureStringVector featureStrings;
  for (auto& fsv : featureStringsView) {
    featureStrings.emplace_back(fsv);
  }
#else
  SettingPathString featuresLower = StringUtils::toLowerCase(strDottedFeatures);
  FeatureStringVector featureStrings = StringUtils::split(featuresLower, '.');
#endif

  FeatureVector features;
  if (!getFeaturePath(featureStrings, features)) {
    return ResultCode::ERR_UNKNOWN_SETTING_PATH;
  }
  path.setFeatures(features);
  return ResultCode::OK;
}

bool
RadioSettings::getFeaturePath(
  const FeatureStringVector& featureStrings,
  FeatureNumVector& featuresOut,
  size_t startIndex
  )
{
  if (startIndex >= featureStrings.size()) {
    return false;
  }

  if (resolvePathForRegisteredSetting<RadioSettings>(featureStrings, featuresOut, startIndex)) {
    return true;
  }

  const FeatureString& key = featureStrings[startIndex];
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
