//
// Created by murray on 25/3/26.
//
#include "ActiveBandSettings.h"

ActiveBandSettings::ActiveBandSettings() :
    SettingsBase()
    ,m_focusBandId(SplitBandId::None)
    ,m_txBandId(SplitBandId::None)
    ,m_rxBandId(SplitBandId::None)
    ,m_split(false)
{
}

BandSettings*
ActiveBandSettings::getBandSettings(SplitBandId idx)
{
  std::optional<BandSettings>& bandOpt = m_activeBands[toSplitIndex(idx)];
  if (bandOpt.has_value()){
    return &bandOpt.value();
  }
  return nullptr;
}

[[nodiscard]] const BandSettings*
ActiveBandSettings::getBandSettings(SplitBandId idx) const
{
  const std::optional<BandSettings>& bandOpt = m_activeBands[toSplitIndex(idx)];
  if (bandOpt.has_value()){
    return &bandOpt.value();
  }
  return nullptr;;
}

bool
ActiveBandSettings::closeBand(SplitBandId idx)
{
  if (idx == SplitBandId::One) {
    if (!m_activeBands[1].has_value()) {
      return false; // Cannot close the only band
    }
    m_activeBands[0] = m_activeBands[1];
    m_changed |= SELECT_1;
  }
  m_activeBands[1].reset();
  m_split = false;
  m_txBandId = SplitBandId::One;
  m_rxBandId = SplitBandId::One;
  m_focusBandId = SplitBandId::One;
  m_changed |= SPLIT | TX_BAND | RX_BAND | FOCUS | SELECT_2;
  BandSettings* bandSettings = getBandSettings(SplitBandId::One);
  bandSettings->markAllChanged(); // Tell the outside world to refresh its copy of this band.
  return true;
}

bool
ActiveBandSettings::applyUpdate(SettingUpdate& update, BandSelector& bandSelector)
{
  if (update.isExhausted()) {
    throw SettingsException("Invalid setting path");
  }
  switch (update.getCurrentFeature()) {
  case REPLACE_FOCUS: return replaceFocusBand(update, bandSelector);
  case SELECT_1: return selectBand(SplitBandId::One, update, bandSelector);
  case SELECT_2: return selectBand(SplitBandId::Two, update, bandSelector);
  default:
    return applyUpdate(update);
  }
}

bool
ActiveBandSettings::applyUpdate(SettingUpdate& update)
{
  uint32_t feature = update.getCurrentFeature();
  const auto& val = update.getValue();

  switch (feature) {

  case WITH_FOCUS: return applyToFocusedBand(update.stepNextFeature());
  case WITH_1: return applyToBand(SplitBandId::One, update.stepNextFeature());
  case WITH_2: return applyToBand(SplitBandId::Two, update.stepNextFeature());
  case FOCUS: return setFocusBand(val);
  case SPLIT: return applySplit(val);
  case TX_BAND: return setTxBand(val);
  case RX_BAND: return setRxBand(val);
  default:
    return false;
  }
}

bool
ActiveBandSettings::getFeaturePath(
  const std::vector<std::string>& featureStrings,
  std::vector<uint32_t>& out,
  size_t startIndex
  )
{
  if (startIndex >= featureStrings.size()) {
    throw SettingsException("Invalid feature path");
  }
  const std::string& key = featureStrings[startIndex];
  if (key == "with-focus") {
    out.push_back(WITH_FOCUS);
    return BandSettings::getFeaturePath(featureStrings, out, startIndex + 1);
  }
  if (key == "with-1") {
    out.push_back(WITH_1);
    return BandSettings::getFeaturePath(featureStrings, out, startIndex + 1);
  }
  if (key == "with-2") {
    out.push_back(WITH_2);
    return BandSettings::getFeaturePath(featureStrings, out, startIndex + 1);
  }
  if (key == "focus") {
    out.push_back(FOCUS);
  }
  if (key == "replace-focus") {
    out.push_back(REPLACE_FOCUS);
    return true;
  }
  if (key == "select-1") {
    out.push_back(SELECT_1);
    return true;
  }
  if (key == "select-2") {
    out.push_back(SELECT_2);
    return true;
  }
  if (key == "split") {
    out.push_back(SPLIT);
    return true;
  }
  if (key == "tx-band") {
    out.push_back(TX_BAND);
    return true;
  }
  if (key == "rx-band") {
    out.push_back(RX_BAND);
    return true;
  }

  return false;
}

[[nodiscard]] SplitBandId
ActiveBandSettings::getSplitBandId(const std::string& bandName) const
{
  if (m_activeBands[0].has_value() && m_activeBands[0].value().getBandName() == bandName) {
    return SplitBandId::One;
  }
  if (m_activeBands[1].has_value() && m_activeBands[1].value().getBandName() == bandName) {
    return SplitBandId::Two;
  }
  return SplitBandId::None;
}

[[nodiscard]] bool
ActiveBandSettings::isBandNameInSelected(const std::string& bandName) const {
  return std::any_of(m_activeBands.begin(), m_activeBands.end(),
    [&](const std::optional<BandSettings>& bandSettings) {
      if (bandSettings.has_value()) {
        return bandSettings.value().getBandName() == bandName;
      }
      return false;
    });
}

bool
ActiveBandSettings::replaceFocusBand(SettingUpdate& update, BandSelector& bandSelector)
{
  const auto& value = update.getValue();
  const std::string replacementBandName = std::get<std::string>(value);
  if (replacementBandName.empty()) {
    return false;
  }
  if (m_focusBandId == SplitBandId::None) {
    m_focusBandId = SplitBandId::One;
  }
  BandSettings* focusBandSettings = getBandSettings(m_focusBandId);
  if (focusBandSettings != nullptr && focusBandSettings->getBandName() == replacementBandName ) {
    return false;
  }
  return selectBand(m_focusBandId, replacementBandName, bandSelector);
}

bool
ActiveBandSettings::selectBand(SplitBandId idx, SettingUpdate& update, BandSelector& bandSelector)
{
  if (idx == SplitBandId::None) {
    return false;
  }
  const auto& value = update.getValue();
  std::optional<std::string> currBandNameOpt = getSplitBandName(idx);

  if (update.getMeaning() == SettingUpdate::VALUE) {
    if (const std::string* pBandName = std::get_if<std::string>(&value)) {
      if (pBandName->empty()) {
        return closeBand(idx);
      } else {
        return selectBand(idx, *pBandName, bandSelector);
      }
    }
  } else if (update.getMeaning() == SettingUpdate::DELTA && currBandNameOpt.has_value()) {
    const Bands& bands = bandSelector.getAllBands();
    if (const int32_t* pDelta = std::get_if<int32_t>(&value)) {
      if (*pDelta > 0) {
        const Band* nextBand = bands.nextBandInOwnCategory(currBandNameOpt.value());
        if (nextBand != nullptr) {
          return selectBand(idx, nextBand->getName(), bandSelector);
        }
      } else if (*pDelta < 0) {
        const Band* prevBand = bands.prevBandInOwnCategory(currBandNameOpt.value());
        if (prevBand != nullptr) {
          return selectBand(idx, prevBand->getName(), bandSelector);
        }
      }
    }
  }
  return false;
}

bool
ActiveBandSettings::selectBand(SplitBandId idx, const std::string& bandName, BandSelector& bandSelector)
{
  if (idx == SplitBandId::None) {
    return false;
  }
  std::optional<std::string> currBandNameOpt = getSplitBandName(idx);
  bool isReplacement = currBandNameOpt.has_value();
  if (isReplacement && bandName == currBandNameOpt.value()) {
    return false; // i.e. No change
  }

  BandSettings* bandSettings = bandSelector.getOrCreateBandSettings(bandName);
  if (bandSettings == nullptr) {
    throw SettingsException("Invalid band name");
  }
  selectBand(idx, *bandSettings);
  // Dirty all the band settings so that external users know to update themselves
  bandSettings->markAllChanged();
  return true;
}

bool
ActiveBandSettings::selectBand(SplitBandId idx, BandSettings& bandSettings)
{
  if (idx == SplitBandId::None) {
    return false;
  }
  if (m_txBandId == SplitBandId::None ) {
    m_txBandId = idx;
    m_changed |= TX_BAND;
  }
  if (m_rxBandId == SplitBandId::None) {
    m_rxBandId = idx;
    m_changed |= RX_BAND;
  }
  if (m_focusBandId == SplitBandId::None){
    m_focusBandId = idx;
    m_changed |= FOCUS;
  }
  m_activeBands.at(toSplitIndex(idx)) = bandSettings;
  m_changed |= idx == SplitBandId::One ? SELECT_1 : SELECT_2;
  return true;
}

bool
ActiveBandSettings::applyToFocusedBand(SettingUpdate& setting)
{
  if (setting.isExhausted()) {
    throw SettingsException("Invalid setting path");
  }
  if (m_focusBandId == SplitBandId::None) {
    throw SettingsException("No band with focus");
  }
  BandSettings* bandSettings = getBandSettings(m_focusBandId);
  if (bandSettings == nullptr) {
    throw SettingsException("Focused band has been removed");
  }
  return bandSettings->applyUpdate(setting);
}

bool
ActiveBandSettings::applyToBand(SplitBandId id, SettingUpdate& setting)
{
  if (setting.isExhausted()) {
    throw SettingsException("Invalid setting path");
  }
  BandSettings* bandSettings = getBandSettings(id);
  if (bandSettings == nullptr) {
    return false;
  }
  return bandSettings->applyUpdate(setting);
}

bool
ActiveBandSettings::applySplit(const SettingValue& value)
{
  const bool* pNewSplit = std::get_if<bool>(&value);
  if (pNewSplit == nullptr) {
    throw SettingsException("split setting should be a boolean");
  }
  if (m_split == *pNewSplit) {
    return false;
  }
  std::optional<BandSettings>& band1Opt = m_activeBands[0];
  std::optional<BandSettings>& band2Opt = m_activeBands[1];
  if (*pNewSplit) {
    if (!band2Opt.has_value()) {
      // Start with both the same
      selectBand(SplitBandId::Two, band1Opt.value());
    }
    if (m_txBandId == SplitBandId::None) {
      m_txBandId = SplitBandId::One;
      m_changed |= TX_BAND;
    }
  } else {
    if (m_focusBandId != SplitBandId::One) {
      m_focusBandId = SplitBandId::One;
      m_changed |= FOCUS;
    }
    if (m_txBandId != SplitBandId::One) {
      m_txBandId = SplitBandId::One;
      m_changed |= TX_BAND;
    }
    if (m_rxBandId != SplitBandId::One) {
      m_rxBandId = SplitBandId::One;
      m_changed |= RX_BAND;
    }
  }
  m_split = *pNewSplit;
  m_changed |= SPLIT;
  return true;
}

bool
ActiveBandSettings::setTxBand(const SettingValue& value)
{
  SplitBandId newTxBandId;
  if (const std::string* pBandName = std::get_if<std::string>(&value)) {
    newTxBandId = getSplitBandId(*pBandName);
  } else if (const SplitBandId* pTxBandId = std::get_if<SplitBandId>(&value)) {
    newTxBandId = *pTxBandId;
  } else {
    throw SettingsException("tx-band setting should be a string or split-band-id");
  }

  if (newTxBandId != SplitBandId::None) {
    if (newTxBandId == m_txBandId) {
      return false; // No change
    }
    m_txBandId = newTxBandId;
    m_changed |= TX_BAND;
    // Dirty the band so that the outside world can respond to their areas of interest.
    BandSettings* newTxBand = getTxBandSettings();
    newTxBand->markAllChanged();
    return true;
  }
  return false;
}

bool
ActiveBandSettings::setRxBand(const SettingValue& value)
{
  SplitBandId newRxBandId;
  if (const std::string* pBandName = std::get_if<std::string>(&value)) {
    newRxBandId = getSplitBandId(*pBandName);
  } else if (const SplitBandId* pRxBandId = std::get_if<SplitBandId>(&value)) {
    newRxBandId = *pRxBandId;
  } else {
    throw SettingsException("rx-band setting should be a string or split-band-id");
  }

  if (newRxBandId != SplitBandId::None) {
    if (newRxBandId == m_rxBandId) {
      return false; // No change
    }
    m_rxBandId = newRxBandId;
    m_changed |= RX_BAND;
    // Dirty the band so that the outside world can respond to their areas of interest.
    BandSettings* newTxBand = getRxBandSettings();
    newTxBand->markAllChanged();
    return true;
  }
  return false;
}

bool
ActiveBandSettings::setFocusBand(const SettingValue& value)
{
  SplitBandId newFocusBandId;
  if (const std::string* pBandName = std::get_if<std::string>(&value)) {
    newFocusBandId = getSplitBandId(*pBandName);
  } else if (const SplitBandId* pFocusBandId = std::get_if<SplitBandId>(&value)) {
    newFocusBandId = *pFocusBandId;
  } else {
    throw SettingsException("rx-band setting should be a string or split-band-id");
  }

  if (newFocusBandId != SplitBandId::None) {
    if (newFocusBandId == m_focusBandId) {
      return false; // No change
    }
    m_focusBandId = newFocusBandId;
    m_changed |= FOCUS;
    // Dirty the band so that the outside world can respond to their areas of interest.
    BandSettings* newFocusBand = getFocusBandSettings();
    newFocusBand->markAllChanged();
    return true;
  }
  return false;
}