//
// Created by murray on 19/1/26.
//

#pragma once
#include <unordered_set>

#include "../base/SettingsBase.h"
#include "Bands.h"
#include "SplitBandId.h"
#include "BandSettings.h"

#define MAX_SPLIT_BANDS 2

class BandSelector : public SettingsBase
{
public:
  enum Features
  {
    NONE = 0,
    REPLACE_FOCUS = 0x001,
    SELECT_1 = 0x002,
    SELECT_2 = 0x004,
    WITH_1 = 0x008,
    WITH_2 = 0x010,
    WITH_FOCUS = 0x020,
    FOCUS = 0x040,
    SPLIT = 0x080,
    TX_BAND = 0x100,
    RX_BAND = 0x200,
    ALL = static_cast<uint32_t>(~0U)
  };

  BandSelector() :
    SettingsBase()
    ,m_split(false)
  {
    initialiseCache();
  }
  BandSelector(const BandSelector& rhs) = default;
  ~BandSelector() override = default;

  void initialiseCache()
  {
    // These will eventually be overridden by settings from JSON
    const std::vector<BandCategory>& categories = m_bands.getCategories();
    for (const auto& category : categories) {
      for (const auto& band : category.getBands()) {
        m_bandSettingsCache.emplace(band.getName(), BandSettings(band));
      }
    }
  }

  static constexpr std::size_t toSplitIndex(SplitBandId b) noexcept {
    return static_cast<std::size_t>(b);
  }

  const Bands& getAllBands() const { return m_bands; }

  bool hasBand(SplitBandId idx) const {
    const auto& nameOpt = m_selectedBandNames[toSplitIndex(idx)];
    return nameOpt.has_value() && !nameOpt->empty();
  }

  std::optional<std::string> getSplitBandName(SplitBandId idx) const
  {
    return m_selectedBandNames.at(toSplitIndex(idx));
  }
  // const std::string& getBand2Name() const { return m_band2Name; }
  const std::string& getTxBandName() const { return m_txBandName; }
  const std::string& getRxBandName() const { return m_rxBandName; }
  const std::string& getFocusBandName() const { return m_focusBandName; }
  bool isSplit() const { return m_split; }

  SplitBandId getFocusBandId() const
  {
    if (m_focusBandName.empty()) {
      return SplitBandId::None;
    }
    if (m_focusBandName == m_selectedBandNames[0]) {
      return SplitBandId::One;
    }
    if (m_focusBandName == m_selectedBandNames[1]) {
      return SplitBandId::Two;
    }
    return SplitBandId::None;
  }
  BandSettings* getFocusBandSettings()
  {
    return getBandSettings(m_focusBandName);
  }

  const BandSettings* getFocusBandSettings() const
  {
    return getBandSettings(m_focusBandName);
  }

  BandSettings* getTxBandSettings()
  {
    return getBandSettings(m_txBandName);
  }

  BandSettings* getRxBandSettings()
  {
    return getBandSettings(m_rxBandName);
  }

  BandSettings* getBandSettings(SplitBandId idx)
  {
    std::optional<std::string>& nameOpt = m_selectedBandNames[toSplitIndex(idx)];
    if (nameOpt.has_value()){
      return getBandSettings(nameOpt.value());
    }
    return nullptr;;
  }

  const BandSettings* getBandSettings(SplitBandId idx) const
  {
    const std::optional<std::string>& nameOpt = m_selectedBandNames[toSplitIndex(idx)];
    if (nameOpt.has_value()){
      return getBandSettings(nameOpt.value());
    }
    return nullptr;;
  }

  bool closeBand(SplitBandId idx)
  {
    if (idx == SplitBandId::One) {
      if (!m_selectedBandNames[1].has_value()) {
        return false; // Cannot close the only band
      }
      m_selectedBandNames[0] = m_selectedBandNames[1];
      m_changed |= SELECT_1;
    }
    m_selectedBandNames[1].reset();
    const std::string& band1Name = m_selectedBandNames[0].value();
    m_split = false;
    m_txBandName = band1Name;
    m_rxBandName = band1Name;
    m_focusBandName = band1Name;
    m_changed |= SPLIT | TX_BAND | RX_BAND | FOCUS | SELECT_2;
    BandSettings* bandSettings = getBandSettings(band1Name);
    bandSettings->setAllChanged(); // Tell the outside world to refresh its copy of this band.
    return true;
  }

  BandSettings* getBandSettings(const std::string& bandName)
  {
    if (m_bandSettingsCache.contains(bandName)) {
      return &m_bandSettingsCache.at(bandName);
    }
    return nullptr;
  }

  const BandSettings* getBandSettings(const std::string& bandName) const
  {
    if (m_bandSettingsCache.contains(bandName)) {
      return &m_bandSettingsCache.at(bandName);
    }
    return nullptr;
  }

  void setCentreFrequencyDeltas(int32_t fine, int32_t coarse)
  {
    for (auto& item : m_bandSettingsCache) {
      item.second.setCentreFrequencyDeltas(fine, coarse);
    }
    m_changed |= WITH_FOCUS;
  }

  void applyRfSettings(const RfSettings& settings, bool onlyChanged)
  {
    for (auto& item : m_bandSettingsCache) {
      item.second.applyRfSettings(settings, onlyChanged);
    }
    m_changed |= WITH_FOCUS;
  }

  void applyIfSettings(const IfSettings& settings)
  {
    for (auto& item : m_bandSettingsCache) {
      item.second.applyIfSettings(settings);
    }
    m_changed |= WITH_FOCUS;
  }

  bool applyUpdate(SettingUpdate& update) override
  {
    if (update.isExhausted()) {
      throw SettingsException("Invalid setting path");
    }
    uint32_t feature = update.getCurrentFeature();
    const auto& val = update.getValue();

    switch (feature) {
    case REPLACE_FOCUS: return replaceFocusBand(update);
    case SELECT_1: return selectBand(SplitBandId::One, update);
    case SELECT_2: return selectBand(SplitBandId::Two, update);
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

  static bool getFeaturePath(
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

protected:

  bool isBandNameInSelected(const std::string& bandName) const {
    return std::any_of(m_selectedBandNames.begin(), m_selectedBandNames.end(),
      [&](const std::optional<std::string>& s) {
        return s.has_value() && *s == bandName;
      });
  }

  bool replaceFocusBand(SettingUpdate& update)
  {
    const auto& value = update.getValue();
    const std::string replacementBandName = std::get<std::string>(value);
    if (replacementBandName.empty() || replacementBandName == m_focusBandName) {
      return false;
    }
    SplitBandId idx = m_focusBandName.empty() ? SplitBandId::One : getFocusBandId();
    return selectBand(idx, replacementBandName);
  }

  bool selectBand(SplitBandId idx, SettingUpdate& update)
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
          return selectBand(idx, *pBandName);
        }
      }
    } else if (update.getMeaning() == SettingUpdate::DELTA && currBandNameOpt.has_value()) {
      if (const int32_t* pDelta = std::get_if<int32_t>(&value)) {
        if (*pDelta > 0) {
          const Band* nextBand = m_bands.nextBandInOwnCategory(currBandNameOpt.value());
          if (nextBand != nullptr) {
            return selectBand(idx, nextBand->getName());
          }
        } else if (*pDelta < 0) {
          const Band* prevBand = m_bands.prevBandInOwnCategory(currBandNameOpt.value());
          if (prevBand != nullptr) {
            return selectBand(idx, prevBand->getName());
          }
        }
      }
    }
    return false;
  }

  bool selectBand(SplitBandId idx, const std::string& bandName)
  {
    if (idx == SplitBandId::None) {
      return false;
    }
    std::optional<std::string> currBandNameOpt = getSplitBandName(idx);
    bool replacingBand = currBandNameOpt.has_value();
    if (replacingBand && bandName == currBandNameOpt.value()) {
      return false; // i.e. No change
    }

    BandSettings* bandSettings = getOrCreateBandSettings(bandName);
    if (bandSettings == nullptr) {
      throw SettingsException("Invalid band name");
    }

    const char * currBandName = replacingBand ? currBandNameOpt.value().c_str() : "";

    if (!replacingBand || currBandName == m_txBandName || m_txBandName.empty()) {
      m_txBandName = bandName;
      m_changed |= TX_BAND;
    }
    if (!replacingBand || currBandName == m_rxBandName || m_rxBandName.empty()) {
      m_rxBandName = bandName;
      m_changed |= RX_BAND;
    }
    if (!replacingBand || (m_focusBandName.empty() || m_focusBandName == currBandName)){
      m_focusBandName = bandName;
      m_changed |= FOCUS;
    }
    m_selectedBandNames.at(toSplitIndex(idx)) = bandName;
    m_changed |= idx == SplitBandId::One ? SELECT_1 : SELECT_2;
    // Dirty all the band settings so that external users know to update themselves
    bandSettings->setAllChanged();
    return true;
  }

  BandSettings* getOrCreateBandSettings(const std::string& bandName)
  {
    BandSettings* bandSettings = getBandSettings(bandName);
    if (bandSettings != nullptr) {
      return bandSettings;
    }
    if (const Band* bandInfo = m_bands.findBand(bandName)) {
      m_bandSettingsCache.emplace(bandName, BandSettings(*bandInfo));
      return &m_bandSettingsCache.at(bandName);
    } else {
      return nullptr;
    }
  }

  bool applyToFocusedBand(SettingUpdate& setting)
  {
    if (setting.isExhausted()) {
      throw SettingsException("Invalid setting path");
    }
    if (m_focusBandName.empty()) {
      throw SettingsException("No band with focus");
    }
    BandSettings* bandSettings = getBandSettings(m_focusBandName);
    if (bandSettings == nullptr) {
      throw SettingsException("Focused band has been removed");
    }
    return bandSettings->applyUpdate(setting);
  }

  bool applyToBand(SplitBandId id, SettingUpdate& setting)
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

  bool applySplit(const SettingValue& value)
  {
    const bool* pNewSplit = std::get_if<bool>(&value);
    if (pNewSplit == nullptr) {
      throw SettingsException("split setting should be a boolean");
    }
    if (m_split == *pNewSplit) {
      return false;
    }
    std::optional<std::string>& band1NameOpt = m_selectedBandNames[0];
    std::optional<std::string>& band2NameOpt = m_selectedBandNames[1];
    if (*pNewSplit) {
      if (!band2NameOpt.has_value()) {
        // band2NameOpt = band1NameOpt.value(); // Start with both the same
        selectBand(SplitBandId::Two, band1NameOpt.value());
      }
      if (m_txBandName.empty()) {
        m_txBandName = band1NameOpt.value();
        m_changed |= TX_BAND;
      }
    } else {
      if (m_focusBandName != band1NameOpt.value()) {
        m_focusBandName = band1NameOpt.value();
        m_changed |= FOCUS;
      }
      if (m_txBandName != band1NameOpt.value()) {
        m_txBandName = band1NameOpt.value();
        m_changed |= TX_BAND;
      }
      if (m_rxBandName != band1NameOpt.value()) {
        m_rxBandName = band1NameOpt.value();
        m_changed |= RX_BAND;
      }
    }
    m_split = *pNewSplit;
    m_changed |= SPLIT;
    return true;
  }

  bool setTxBand(const SettingValue& value)
  {
    std::string txBandName;
    if (const std::string* pBandName = std::get_if<std::string>(&value)) {
      txBandName = *pBandName;
    } else if (const SplitBandId* pTxBandId = std::get_if<SplitBandId>(&value)) {
      std::optional<std::string> bandNameOpt = getSplitBandName(*pTxBandId);
      if (bandNameOpt.has_value()) {
        txBandName = bandNameOpt.value();
      }
    }

    if (!txBandName.empty()) {
      if (m_txBandName == txBandName) {
        return false;
      }
      if (!isBandNameInSelected(txBandName)) {
        throw SettingsException("tx-band setting refers to an unselected band");
      }
      m_txBandName = txBandName;
      m_changed |= TX_BAND;
      // Dirty the band so that the outside world can respond to their areas of interest.
      BandSettings* newTxBand = getTxBandSettings();
      newTxBand->setAllChanged();
      return true;
    }
    return false;
  }

  bool setRxBand(const SettingValue& value)
  {
    if (const std::string* pNewRxBandName = std::get_if<std::string>(&value)) {
      if (m_rxBandName == *pNewRxBandName) {
        return false;
      }
      if (!isBandNameInSelected(*pNewRxBandName)) {
        throw SettingsException("rx-band setting refers to an unselected band");
      }
      m_rxBandName = *pNewRxBandName;
      m_changed |= RX_BAND;
      return true;
    }
    throw SettingsException("tx-band setting should be a string");
  }

  bool setFocusBand(const SettingValue& value)
  {
    std::string focusBandName;
    if (const std::string* pBandName = std::get_if<std::string>(&value)) {
      focusBandName = *pBandName;
    } else if (const SplitBandId* pFocusBandId = std::get_if<SplitBandId>(&value)) {
      std::optional<std::string> bandNameOpt = getSplitBandName(*pFocusBandId);
      if (bandNameOpt.has_value()) {
        focusBandName = bandNameOpt.value();
      }
    }
    if (!focusBandName.empty()) {
      if (m_focusBandName == focusBandName) {
        return false;
      }
      if (!isBandNameInSelected(focusBandName)) {
        throw SettingsException("focus setting refers to an unselected band");
      }
      m_focusBandName = focusBandName;
      m_changed |= FOCUS;
      // Dirty the band so that the outside world can respond to their areas of interest.
      BandSettings* newFocusBand = getFocusBandSettings();
      newFocusBand->setAllChanged();
      return true;
    }
    return false;
  }

  Bands m_bands;
  std::unordered_map<std::string, BandSettings> m_bandSettingsCache;
  bool m_split;
  std::array<std::optional<std::string>, MAX_SPLIT_BANDS> m_selectedBandNames;
  // std::string m_band1Name;
  // std::string m_band2Name;
  std::string m_txBandName;
  std::string m_rxBandName;
  std::string m_focusBandName;
};