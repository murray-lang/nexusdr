//
// Created by murray on 15/04/23.
//

#pragma once


#include "ModeSettings.h"
#include "ReceiverSettings.h"
#include "TransmitterSettings.h"
#include "base/SettingUpdatePath.h"
#include <cstdint>
#include <string>
#include <vector>

// #include "bands/Band.h"
// #include "./bands/Bands.h"
#include "bands/BandSettings.h"
#include "bands/BandSelector.h"
#include "base/SettingsException.h"
#include "../util/StringUtils.h"


class RadioSettings : public SettingsBase {
public:
  enum Features
  {
    NONE = 0,
    PTT = 0x01,
    BAND = 0x02,
    TX = 0x04,
    RX = 0x08,
    ALL = static_cast<uint32_t>(~0U)
  };

  RadioSettings() : m_ptt(this, "ptt", false)
  {
    RadioSettings::setAllChanged();
  };

  RadioSettings(const RadioSettings& rhs) : 
  SettingsBase(rhs),
  m_ptt(this, rhs.m_ptt),
  // m_bandName(this, rhs.m_bandName),
  m_rxSettings(rhs.m_rxSettings),
  m_txSettings(rhs.m_txSettings)
  {
  }
  ~RadioSettings() override = default;

  RadioSettings& operator=(const RadioSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      // m_bandName = rhs.m_bandName;
      m_rxSettings = rhs.m_rxSettings;
      m_txSettings = rhs.m_txSettings;
    }
    return *this;
  }

  BandSelector& getBandSelector() { return m_bandSelector; }

  [[nodiscard]] bool getPtt() const { return m_ptt(); }
  // [[nodiscard]] const std::string& getBandName() const { return m_bandName(); }
  [[nodiscard]] const ReceiverSettings& getRxSettings() const { return m_rxSettings; }
  [[nodiscard]] const TransmitterSettings& getTxSettings() const { return m_txSettings; }

  static void setCentreFrequencyDeltas(int32_t fine, int32_t coarse);
  static BandSettings* getBandSettings(const std::string& bandName);
  static BandSettings* getFocusBandSettings();
  static const std::string& getFocusBandName() { return m_bandSelector.getFocusBandName(); }

  static const Bands& getBands() { return m_bandSelector.getAllBands(); }

  void applyRfSettings(const RfSettings& settings, bool onlyChanged = false);
  void applyIfSettings(const IfSettings& settings);

  bool applyUpdate(SettingUpdate& update) override;

  void clearChanged() override
  {
    SettingsBase::clearChanged();
    m_rxSettings.clearChanged();
    m_txSettings.clearChanged();
  }

  void setAllChanged() override
  {
    SettingsBase::setAllChanged();
    m_rxSettings.setAllChanged();
    m_txSettings.setAllChanged();
    // Not PTT! That being set will short-circuit all other changes
    m_changed &= ~PTT;
  }

  RxPipelineSettings* getFocusPipeline();
  [[nodiscard]] const RxPipelineSettings* getFocusPipeline() const;
  TxPipelineSettings* getTxPipelineSettings();
  [[nodiscard]] const TxPipelineSettings* getTxPipelineSettings() const;
  [[nodiscard]] const Mode* getFocusRxPipelineMode() const;

  static SettingUpdatePath getSettingUpdatePath(const std::string& strDottedFeatures);

  // ---- New: canonical SettingUpdate factories (UI should use these) ----
  static SettingUpdate makeBandSetMultiPipelineUpdate(SplitBandId whichBand, bool enable);
  static SettingUpdate makeBandClosePipelineUpdate(SplitBandId whichBand, PipelineId whichPipeline);
  static SettingUpdate makeBandSetTxPipelineUpdate(SplitBandId whichBand, PipelineId whichPipeline);
  static SettingUpdate makeBandSetFocusPipelineUpdate(SplitBandId whichBand, PipelineId whichPipeline);
  static SettingUpdate makeBandSetSetTxBandUpdate(SplitBandId whichBand);

  // Applies to the *focused* pipeline within that band (assumes focus is already correct).
  static SettingUpdate makeBandSetModeOnFocusPipelineUpdate(SplitBandId whichBand, Mode::Type modeType);

  static bool getFeaturePath(
    const std::vector<std::string>& featureStrings,
    std::vector<uint32_t>& featuresOut,
    size_t startIndex = 0
    );
protected:
  static uint32_t toWithBandFeature(SplitBandId whichBand);

  Setting<bool, PTT, RadioSettings> m_ptt;
  // Setting<std::string, BAND, RadioSettings> m_bandName;
  ReceiverSettings m_rxSettings;
  TransmitterSettings m_txSettings;
  static BandSelector m_bandSelector;
};
