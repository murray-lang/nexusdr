//
// Implementation of BandSettings
//

#include "BandSettings.h"

BandSettings::BandSettings()
  : m_pipelineA({})
  , m_txPipeline({})
  , m_multi(this, "split-pipelines", false)
  , m_focusId(this, "focus-pipeline", PipelineId::A)
  , m_txId(this, "tx-pipeline", PipelineId::A)
{
  BandSettings::setAllChanged();
}

BandSettings::BandSettings(const BandSettings& rhs)
  : SettingsBase(rhs)
  , m_band(rhs.m_band)
  , m_pipelineA(rhs.m_pipelineA)
  , m_pipelineB(rhs.m_pipelineB)
  , m_txPipeline(rhs.m_txPipeline)
  , m_multi(this, rhs.m_multi)
  , m_focusId(this, rhs.m_focusId)
  , m_txId(this, rhs.m_txId)
{
}

BandSettings::BandSettings(const Band& band)
  : m_band(band)
  , m_pipelineA({})
  , m_multi(this, "split-pipelines", false)
  , m_focusId(this, "focus-pipeline", PipelineId::A)
  , m_txId(this, "tx-pipeline", PipelineId::A)
{
  // addRxPipeline();
  applyBandDefaults(band);
  BandSettings::setAllChanged();
}

BandSettings& BandSettings::operator=(const BandSettings& rhs)
{
  if (this != &rhs) {
    SettingsBase::operator=(rhs);
    m_band = rhs.m_band;
    m_pipelineA = rhs.m_pipelineA;
    m_pipelineB = rhs.m_pipelineB;
    m_multi = rhs.m_multi;
    m_focusId = rhs.m_focusId;
    m_txId = rhs.m_txId;
  }
  return *this;
}

bool
BandSettings::splitPipelines(bool split)
{
  if (m_multi() == split) {
    return false;
  }
  m_multi.apply(split);
  if (split) {
    m_pipelineB = m_pipelineA;
    m_pipelineB.value().pivotFrequencyAroundCentre();
    m_focusId.apply(PipelineId::B);
    m_txId.apply(PipelineId::B);
    m_txPipeline.copyBasicsForTracking(m_pipelineB.value());
    m_changed |= MULTI_PIPELINE | FOCUS_PIPELINE | TX_PIPELINE;
    return true;
  }
  return closePipeline(m_focusId());
}

bool
BandSettings::closePipeline(PipelineId id)
{
  if (id == PipelineId::A) {
    m_pipelineA = m_pipelineB.value();

  } else if (!m_pipelineB.has_value()) {
    return false;
  }
  m_txPipeline.copyBasicsForTracking(m_pipelineA);
  m_txId.apply(PipelineId::A);
  m_focusId.apply(PipelineId::A);
  m_multi.apply(false);
  m_pipelineB.reset();
  m_changed |= MULTI_PIPELINE | FOCUS_PIPELINE | TX_PIPELINE | CLOSE_PIPELINE;
  return true;
}

RxPipelineSettings*
BandSettings::getFocusPipeline()
{
  return getRxPipeline(m_focusId());
}

const
RxPipelineSettings* BandSettings::getFocusPipeline() const
{
  return getRxPipeline(m_focusId());
}

TxPipelineSettings*
BandSettings::getTxPipeline()
{
  return &m_txPipeline;
}

const TxPipelineSettings*
BandSettings::getTxPipeline() const
{
  return &m_txPipeline;
}

RxPipelineSettings*
BandSettings::getRxPipeline(PipelineId id)
{
  if (id == PipelineId::A) {
    return &m_pipelineA;
  }
  if (m_pipelineB.has_value() && id == PipelineId::B) {
    return &m_pipelineB.value();
  }
  return nullptr;
}

const RxPipelineSettings*
BandSettings::getRxPipeline(PipelineId id) const
{
  if (id == PipelineId::A) {
    return &m_pipelineA;
  }
  if (m_pipelineB.has_value() && id == PipelineId::B) {
    return &m_pipelineB.value();
  }
  return nullptr;
}

RfSettings*
BandSettings::getFocusRxRfSettings()
{
  RxPipelineSettings* focusPipeline = getFocusPipeline();
  if (focusPipeline != nullptr) {
    return &focusPipeline->getRfSettings();
  }
  return nullptr;
}

const RfSettings*
BandSettings::getFocusRxRfSettings() const
{
  const RxPipelineSettings* focusPipeline = getFocusPipeline();
  if (focusPipeline != nullptr) {
    return &focusPipeline->getRfSettings();
  }
  return nullptr;
}

IfSettings*
BandSettings::getFocusRxIfSettings()
{
  RxPipelineSettings* focusPipeline = getFocusPipeline();
  if (focusPipeline != nullptr) {
    return &focusPipeline->getIfSettings();
  }
  return nullptr;
}

const Mode*
BandSettings::getFocusRxMode() const
{
  const RxPipelineSettings* focusPipeline = getFocusPipeline();
  if (focusPipeline != nullptr) {
    return &focusPipeline->getMode();
  }
  return nullptr;
}

const IfSettings*
BandSettings::getFocusRxIfSettings() const
{
  const RxPipelineSettings* focusPipeline = getFocusPipeline();
  if (focusPipeline != nullptr) {
    return &focusPipeline->getIfSettings();
  }
  return nullptr;
}

void BandSettings::clearChanged()
{
  SettingsBase::clearChanged();
  m_txPipeline.clearChanged();
  m_pipelineA.clearChanged();
  if (m_pipelineB.has_value()) {
    m_pipelineB.value().clearChanged();
  }
}

void BandSettings::setAllChanged()
{
  SettingsBase::setAllChanged();
  m_txPipeline.setAllChanged();
  m_pipelineA.setAllChanged();
  if (m_pipelineB.has_value()) {
    m_pipelineB.value().setAllChanged();
  }
}

std::string BandSettings::getBandName() const
{
  return m_band.getName();
}

void BandSettings::applyBandDefaults(const Band& band)
{
  m_txPipeline.applyBandDefaults(&band);
  m_pipelineA.applyBandDefaults(&band);
  if (m_pipelineB.has_value()) {
    m_pipelineB.value().applyBandDefaults(&band);
  }
}

bool BandSettings::setMode(const Mode& mode)
{
  bool changed = false;
  RxPipelineSettings* focusPipeline = getFocusPipeline();
  if (focusPipeline != nullptr) {
    changed |= focusPipeline->setMode(mode);
    if (m_focusId() == m_txId()) {
      changed |= m_txPipeline.setMode(mode);
    }
  }
  return changed;
}

void BandSettings::setCentreFrequencyDeltas(int32_t fine, int32_t coarse)
{
  m_txPipeline.setCentreFrequencyDeltas(fine, coarse);
  m_pipelineA.setCentreFrequencyDeltas(fine, coarse);
  if (m_pipelineB.has_value()) {
    m_pipelineB.value().setCentreFrequencyDeltas(fine, coarse);
  }
}

void BandSettings::applyRfSettings(const RfSettings& settings, bool onlyChanged)
{
  m_txPipeline.mergeRfSettings(settings, onlyChanged);
  m_pipelineA.mergeRfSettings(settings, onlyChanged);
  if (m_pipelineB.has_value()) {
    m_pipelineB.value().mergeRfSettings(settings, onlyChanged);
  }
}

void BandSettings::applyIfSettings(const IfSettings& settings)
{
  m_pipelineA.mergeIfSettings(settings);
  if (m_pipelineB.has_value()) {
    m_pipelineB.value().mergeIfSettings(settings);
  }
}

bool BandSettings::applyUpdate(SettingUpdate& update)
{
  if (update.isExhausted()) {
    throw SettingsException("Invalid setting path");
  }
  uint32_t feature = update.getCurrentFeature();
  const auto& val = update.getValue();

  switch (feature) {
  case MULTI_PIPELINE:
    return applySplitPipelines(val);
  case FOCUS_PIPELINE:
    return setFocusPipeline(val);
  case WITH_FOCUS_PIPELINE:
    update.stepNextFeature();
    return withFocusPipeline(update);
  case TX_PIPELINE:
    return setTxPipeline(val);
  case WITH_TX_PIPELINE:
    update.stepNextFeature();
    return withTxPipeline(update);
  case CLOSE_PIPELINE:
    return applyClosePipeline(val);
  default:
    return false;
  }
}

bool BandSettings::getFeaturePath(
  const std::vector<std::string>& featureStrings,
  std::vector<uint32_t>& featuresOut,
  size_t startIndex)
{
  if (startIndex >= featureStrings.size()) {
    throw SettingsException("Invalid feature path");
  }
  if (resolvePathForRegisteredSetting<BandSettings>(featureStrings, featuresOut, startIndex)) {
    return true;
  }
  const std::string& key = featureStrings[startIndex];
  if (key == "split-pipelines") {
    featuresOut.push_back(MULTI_PIPELINE);
    return RxPipelineSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
  }
  if (key == "with-focus-pipeline") {
    featuresOut.push_back(WITH_FOCUS_PIPELINE);
    return RxPipelineSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
  }
  if (key == "with-tx-pipeline") {
    featuresOut.push_back(WITH_TX_PIPELINE);
    return true;
  }
  if (key == "close-pipeline") {
    featuresOut.push_back(CLOSE_PIPELINE);
    return true;
  }
  return false;
}


RfSettings& BandSettings::getTxRfSettings()
{
  return m_txPipeline.getRfSettings();
}

const RfSettings& BandSettings::getTxRfSettings() const
{
  return m_txPipeline.getRfSettings();
}

bool
BandSettings::applySplitPipelines(const SettingValue& settingValue)
{
  return splitPipelines(std::get<bool>(settingValue));
}

bool
BandSettings::applyClosePipeline(const SettingValue& settingValue)
{
  return closePipeline(std::get<PipelineId>(settingValue));
}

bool
BandSettings::setTxPipeline(const SettingValue& settingValue)
{
  PipelineId newTxId = std::get<PipelineId>(settingValue);
  if (newTxId == m_txId()) {
    return false;
  }
  if (newTxId == PipelineId::B && !m_multi()) {
    return false;
  }
  RxPipelineSettings* rxPipeline = getRxPipeline(newTxId);
  if (rxPipeline == nullptr) {
    return false;
  }
  m_txPipeline.copyBasicsForTracking(*rxPipeline);
  m_txId.apply(newTxId);
  return true;
}

bool
BandSettings::setFocusPipeline(const SettingValue& settingValue)
{
  PipelineId newFocusId = std::get<PipelineId>(settingValue);
  if (newFocusId == m_focusId()) {
    return false;
  }
  if (newFocusId == PipelineId::B && !m_multi()) {
    return false;
  }
  RxPipelineSettings* rxPipeline = getRxPipeline(newFocusId);
  if (rxPipeline == nullptr) {
    return false;
  }
  if (m_focusId() == m_txId()) {
    m_txPipeline.copyBasicsForTracking(*rxPipeline);
    m_txId.apply(newFocusId);
  }
  m_focusId.apply(newFocusId);
  return true;
}

bool BandSettings::withFocusPipeline(SettingUpdate& setting)
{
  RxPipelineSettings* focusPipelineSettings = getFocusPipeline();
  if (focusPipelineSettings == nullptr) {
    return false;
  }
  if (focusPipelineSettings->applyUpdate(setting)) {
    if (m_focusId() == m_txId()) {
      if (focusPipelineSettings->hasSettingChanged(PipelineSettings::RF)) {
        m_txPipeline.getRfSettings().copyFrequencies(focusPipelineSettings->getRfSettings());
        m_changed |= WITH_TX_PIPELINE;
      }
      if (focusPipelineSettings->hasSettingChanged(PipelineSettings::MODE)) {
        m_txPipeline.setMode(focusPipelineSettings->getMode());
        m_changed |= WITH_TX_PIPELINE;
      }
    }
    m_changed |= WITH_FOCUS_PIPELINE;
    return true;
  }
  return false;
}

bool BandSettings::withTxPipeline(SettingUpdate& setting)
{
  if (m_txPipeline.applyUpdate(setting)) {
    m_changed |= WITH_TX_PIPELINE;
    return true;
  }
  return false;
}
