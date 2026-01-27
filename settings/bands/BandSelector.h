//
// Created by murray on 19/1/26.
//

#pragma once
#include <unordered_set>

#include "../core/SettingsBase.h"
#include "Bands.h"
#include "BandSettings.h"

#define MAX_SPLIT_BANDS 2

class BandSelector : public SettingsBase
{
public:
  enum Features
  {
    NONE = 0,
    SELECT = 0x01,
    SELECT_A = SELECT,
    SELECT_B = 0x02,
    SELECTED = 0x04,
    SPLIT = 0x08,
    TX_BAND = 0x10,
    FOCUS_AB   = 0x20,
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

  const Bands& getBands() const { return m_bands; }
  const std::string& getBandAName() const { return m_bandAName; }
  const std::string& getBandBName() const { return m_bandBName; }
  const std::string& getTxBandName() const { return m_txBandName; }
  const std::string& getFocusBandName() const { return m_focusBandName; }
  bool isSplit() const { return m_split; }

  BandSettings* getFocusBandSettings()
  {
    return getBandSettings(m_focusBandName);
  }

  const BandSettings* getFocusBandSettings() const
  {
    return getBandSettings(m_focusBandName);
  }

  BandSettings* getBandASettings()
  {
    return getBandSettings(m_bandAName);
  }

  BandSettings* getBandBSettings()
  {
    return getBandSettings(m_bandAName);
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
    m_changed |= SELECTED;
  }

  void applyRfSettings(const RfSettings& settings, bool onlyChanged)
  {
    for (auto& item : m_bandSettingsCache) {
      item.second.applyRfSettings(settings, onlyChanged);
    }
    m_changed |= SELECTED;
  }

  void applyIfSettings(const IfSettings& settings)
  {
    for (auto& item : m_bandSettingsCache) {
      item.second.applyIfSettings(settings);
    }
    m_changed |= SELECTED;
  }

  bool applyUpdate(SettingUpdate& update) override
  {
    if (update.isExhausted()) {
      throw SettingsException("Invalid setting path");
    }
    uint32_t feature = update.getCurrentFeature();
    const auto& val = update.getValue();

    switch (feature) {
    case SELECT_A: return selectBandA(update);
    case SELECT_B: return selectBandB(update);
    case SELECTED: return applyToFocusedBand(update.stepNextFeature());
    case SPLIT: return applySplit(val);
    case TX_BAND: return applyTxBand(val);
    case FOCUS_AB: return applyFocusAb(val);
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
    if (key == "selected") {
      out.push_back(SELECTED);
      return BandSettings::getFeaturePath(featureStrings, out, startIndex + 1);
    }
    if (key == "select") {
      out.push_back(SELECT);
      return true;
    }
    if (key == "select-a") {
      out.push_back(SELECT_A);
      return true;
    }
    if (key == "select-b") {
      out.push_back(SELECT_B);
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
    if (key == "focus-ab") {
      out.push_back(FOCUS_AB);
    }
    return false;
  }

protected:

  bool selectBandA(SettingUpdate& update)
  {
    const auto& value = update.getValue();
    if (update.getMeaning() == SettingUpdate::VALUE) {
      if (const std::string* pBandName = std::get_if<std::string>(&value)) {
        return selectBandA(*pBandName);
      }
    } else if (update.getMeaning() == SettingUpdate::DELTA) {
      if (const int32_t* pDelta = std::get_if<int32_t>(&value)) {
        if (*pDelta > 0) {
          const Band* nextBand = m_bands.nextBandInOwnCategory(m_bandAName);
          if (nextBand != nullptr) {
            return selectBandA(nextBand->getName());
          }
        } else if (*pDelta < 0) {
          const Band* prevBand = m_bands.prevBandInOwnCategory(m_bandAName);
          if (prevBand != nullptr) {
            return selectBandA(prevBand->getName());
          }
        }
      }
    }
    return false;
  }

  bool selectBandB(SettingUpdate& update)
  {
    const auto& value = update.getValue();
    if (update.getMeaning() == SettingUpdate::VALUE) {
      if (const std::string* pBandName = std::get_if<std::string>(&value)) {
        return selectBandB(*pBandName);
      }
    } else if (update.getMeaning() == SettingUpdate::DELTA) {
      if (const int32_t* pDelta = std::get_if<int32_t>(&value)) {
        if (*pDelta > 0) {
          const Band* nextBand = m_bands.nextBandInOwnCategory(m_bandAName);
          if (nextBand != nullptr) {
            return selectBandB(nextBand->getName());
          }
        } else if (*pDelta < 0) {
          const Band* prevBand = m_bands.prevBandInOwnCategory(m_bandAName);
          if (prevBand != nullptr) {
            return selectBandB(prevBand->getName());
          }
        }
      }
    }
    return false;
  }

  bool selectBandA(const std::string& bandName)
  {
    if (bandName == m_bandAName) {
      return false; // i.e. No change
    }
    BandSettings* bandSettings = getOrCreateBandSettings(bandName);
    if (bandSettings == nullptr) {
      throw SettingsException("Invalid band name");
    }
    if (m_bandAName == m_txBandName) {
      m_txBandName = bandName;
      m_changed |= TX_BAND;
    }
    if (m_focusBandName.empty() || m_focusBandName == m_bandAName) {
      m_focusBandName = bandName;
      m_changed |= FOCUS_AB;
    }
    m_bandAName = bandName;
    m_changed |= SELECT_A;
    // Dirty all the band settings so that external users know to update themselves
    bandSettings->setAllChanged();
    return true;
  }

  bool selectBandB(const std::string& bandName)
  {
    if (bandName == m_bandBName) {
      return false; // i.e. No change
    }
    BandSettings* bandSettings = getOrCreateBandSettings(bandName);
    if (bandSettings == nullptr) {
      throw SettingsException("Invalid band name");
    }
    if (m_bandBName == m_txBandName) {
      m_txBandName = bandName;
      m_changed |= TX_BAND;
    }
    if (m_focusBandName.empty() || m_focusBandName == m_bandBName) {
      m_focusBandName = bandName;
      m_changed |= FOCUS_AB;
    }
    m_bandBName = bandName;
    m_changed |= SELECT_B;
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
      throw SettingsException("No band selected");
    }
    BandSettings* bandSettings = getBandSettings(m_focusBandName);
    if (bandSettings == nullptr) {
      throw SettingsException("Focused band has been removed");
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
    if (m_split) {
      if (m_bandBName.empty()) {
        m_bandBName = m_bandAName; // Start with both the same
        m_changed |= SELECT_B;
      }
      if (m_txBandName.empty()) {
        m_txBandName = m_bandAName;
        m_changed |= TX_BAND;
      }
    } else {
      if (m_focusBandName != m_bandAName) {
        m_focusBandName = m_bandAName;
        m_changed |= FOCUS_AB;
      }
      if (m_txBandName != m_bandAName) {
        m_txBandName = m_bandAName;
        m_changed |= TX_BAND;
      }
    }
    m_split = *pNewSplit;
    m_changed |= SPLIT;
    return true;
  }

  bool applyTxBand(const SettingValue& value)
  {
    if (const std::string* pNewTxBandName = std::get_if<std::string>(&value)) {
      if (m_txBandName == *pNewTxBandName) {
        return false;
      }
      if (*pNewTxBandName != m_bandAName && *pNewTxBandName != m_bandBName) {
        throw SettingsException("tx-band setting refers to an unselected band");
      }
      m_txBandName = *pNewTxBandName;
      m_changed |= TX_BAND;
      return true;
    }
    throw SettingsException("tx-band setting should be a string");
  }

  bool applyFocusAb(const SettingValue& value)
  {
    if (const std::string* pBandName = std::get_if<std::string>(&value)) {
      if (m_focusBandName == *pBandName) {
        return false;
      }
      if (*pBandName != m_bandAName && *pBandName != m_bandBName) {
        throw SettingsException("focus-ab setting refers to an unselected band");
      }
      m_focusBandName = *pBandName;
      m_changed |= FOCUS_AB;
      return true;
    }
    return false;
  }

  Bands m_bands;
  std::unordered_map<std::string, BandSettings> m_bandSettingsCache;
  bool m_split;
  std::string m_bandAName;
  std::string m_bandBName;
  std::string m_txBandName;
  std::string m_focusBandName;
};