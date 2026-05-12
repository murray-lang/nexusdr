#pragma once
#include "../base/SettingsBase.h"
#include "Bands.h"
#include "BandSelector.h"
#include "SplitBandId.h"
#include "BandSettings.h"

#define MAX_ACTIVE_BANDS 2

#ifdef USE_ETL
#include "etl/array.h"
using ActiveBandSettingsArray = etl::array<etl::optional<BandSettings>, MAX_ACTIVE_BANDS>;
#else
#include <array>

using ActiveBandSettingsArray = std::array<std::optional<BandSettings>, MAX_ACTIVE_BANDS>;
#endif

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

  void clearChanged() override
  {
    SettingsBase::clearChanged();
    for (auto& bandSettings : m_activeBands) {
      if (bandSettings) {
        bandSettings->clearChanged();
      }
    }
  }

  void markAllChanged() override;

  static constexpr size_t toSplitIndex(SplitBandId b) noexcept {
    return static_cast<size_t>(b);
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

  [[nodiscard]] optional<BandNameString> getSplitBandName(SplitBandId idx) const
  {
    optional<BandSettings> bandSettings = m_activeBands.at(toSplitIndex(idx));
    if (bandSettings) {
      return bandSettings->getBandName();
    }
    return {};
  }

  [[nodiscard]] optional<BandNameString> getFocusBandName() { return getSplitBandName(m_focusBandId); }
  [[nodiscard]] optional<BandNameString> getFocusBandName() const { return getSplitBandName(m_focusBandId); }
  [[nodiscard]] optional<BandNameString> getTxBandName() const { return getSplitBandName(m_txBandId); }
  [[nodiscard]] optional<BandNameString> getRxBandName() const { return getSplitBandName(m_rxBandId); }

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
    const FeatureStringVector& featureStrings,
    FeatureNumVector& out,
    size_t startIndex
    );

protected:
  [[nodiscard]] SplitBandId getSplitBandId(const BandNameString& bandName) const;
  [[nodiscard]] bool isBandNameInSelected(const BandNameString& bandName) const ;
  bool replaceFocusBand(SettingUpdate& update, BandSelector& bandSelector);
  bool selectBand(SplitBandId idx, SettingUpdate& update, BandSelector& bandSelector);
  bool selectBand(SplitBandId idx, const BandNameString& bandName, BandSelector& bandSelector);
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
  ActiveBandSettingsArray m_activeBands;
  SplitBandId m_focusBandId;
  SplitBandId m_txBandId;
  SplitBandId m_rxBandId;
};