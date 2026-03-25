//
// Created by murray on 19/1/26.
//

#pragma once
#include <unordered_set>

#include "../base/SettingsBase.h"
#include "Bands.h"
#include "BandSelector.h"
#include "SplitBandId.h"
#include "BandSettings.h"

#define MAX_ACTIVE_BANDS 2

class ActiveBandSettings : public SettingsBase
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

  ActiveBandSettings();
  ActiveBandSettings(const ActiveBandSettings& rhs) = default;
  ~ActiveBandSettings() override = default;

  static constexpr std::size_t toSplitIndex(SplitBandId b) noexcept {
    return static_cast<std::size_t>(b);
  }

  // [[nodiscard]] const Bands& getAllBands() const
  // {
  //   return m_bandSelector.getAllBands();
  // }

  [[nodiscard]] bool hasBand(SplitBandId idx) const {
    const auto& nameOpt = m_activeBands[toSplitIndex(idx)];
    return nameOpt.has_value();
  }

  [[nodiscard]] bool isSplit() const { return m_split; }

  [[nodiscard]] std::optional<std::string> getSplitBandName(SplitBandId idx) const
  {
    std::optional<BandSettings> bandSettings = m_activeBands.at(toSplitIndex(idx));
    if (bandSettings.has_value()) {
      return bandSettings.value().getBandName();
    }
    return {};
  }

  [[nodiscard]] std::optional<std::string> getFocusBandName() { return getSplitBandName(m_focusBandId); }
  [[nodiscard]] std::optional<std::string> getFocusBandName() const { return getSplitBandName(m_focusBandId); }
  [[nodiscard]] std::optional<std::string> getTxBandName() const { return getSplitBandName(m_txBandId); }
  [[nodiscard]] std::optional<std::string> getRxBandName() const { return getSplitBandName(m_rxBandId); }

  [[nodiscard]] SplitBandId getFocusBandId() const { return m_focusBandId; }
  [[nodiscard]] SplitBandId getTxBandId() const { return m_txBandId; }
  [[nodiscard]] SplitBandId getRxBandId() const { return m_rxBandId; }

  BandSettings* getFocusBandSettings() { return getBandSettings(m_focusBandId); }
  [[nodiscard]] const BandSettings* getFocusBandSettings() const { return getBandSettings(m_focusBandId); }
  BandSettings* getTxBandSettings() { return getBandSettings(m_txBandId); }
  BandSettings* getRxBandSettings() { return getBandSettings(m_rxBandId); }
  BandSettings* getBandSettings(SplitBandId idx);
  [[nodiscard]] const BandSettings* getBandSettings(SplitBandId idx) const;
  bool closeBand(SplitBandId idx);
  bool applyUpdate(SettingUpdate& update, BandSelector& bandSelector);
  bool applyUpdate(SettingUpdate& update) override;
  static bool getFeaturePath(
    const std::vector<std::string>& featureStrings,
    std::vector<uint32_t>& out,
    size_t startIndex
    );

protected:
  [[nodiscard]] SplitBandId getSplitBandId(const std::string& bandName) const;
  [[nodiscard]] bool isBandNameInSelected(const std::string& bandName) const ;
  bool replaceFocusBand(SettingUpdate& update, BandSelector& bandSelector);
  bool selectBand(SplitBandId idx, SettingUpdate& update, BandSelector& bandSelector);
  bool selectBand(SplitBandId idx, const std::string& bandName, BandSelector& bandSelector);
  bool selectBand(SplitBandId idx, BandSettings& bandSettings);
  bool applyToFocusedBand(SettingUpdate& setting);
  bool applyToBand(SplitBandId id, SettingUpdate& setting);
  bool applySplit(const SettingValue& value);
  bool setTxBand(const SettingValue& value);
  bool setRxBand(const SettingValue& value);
  bool setFocusBand(const SettingValue& value);

  // Bands m_bands;
  // std::unordered_map<std::string, BandSettings> m_bandSettingsCache;
  bool m_split;
  // std::array<std::optional<std::string>, MAX_ACTIVE_BANDS> m_selectedBandNames;
  // std::string m_txBandName;
  // std::string m_rxBandName;
  // std::string m_focusBandName;
  std::array<std::optional<BandSettings>, MAX_ACTIVE_BANDS> m_activeBands;
  SplitBandId m_focusBandId;
  SplitBandId m_txBandId;
  SplitBandId m_rxBandId;
};