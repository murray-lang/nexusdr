//
// Created by murray on 15/04/23.
//

#pragma once


#include "ModeSettings.h"
#include "ReceiverSettings.h"
#include "TransmitterSettings.h"
#include "base/SettingUpdatePath.h"
#include <cstdint>


#include "ResultCode.h"
#include "bands/BandSettings.h"
#include "bands/ActiveBandSettings.h"
#include "bands/BandSelector.h"

#ifdef USE_ETL_COLLECTIONS
#include <etl/string.h>
#include <etl/vector.h>
#else
#include <string>
#include <vector>
#endif


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

  RadioSettings() :
    m_activeBandSettings(),
    m_ptt(this, "ptt", false)
  {
    RadioSettings::markAllChanged();
  };

  RadioSettings(const RadioSettings& rhs) :
    SettingsBase(rhs),
    m_activeBandSettings(rhs.m_activeBandSettings),
    m_ptt(this, rhs.m_ptt),
    m_rxSettings(rhs.m_rxSettings),
    m_txSettings(rhs.m_txSettings)
  {
  }
  ~RadioSettings() override = default;

  RadioSettings& operator=(const RadioSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      m_activeBandSettings = rhs.m_activeBandSettings;
      m_rxSettings = rhs.m_rxSettings;
      m_txSettings = rhs.m_txSettings;
    }
    return *this;
  }

  ActiveBandSettings& getActiveBandSettings() { return m_activeBandSettings; }

  [[nodiscard]] bool getPtt() const { return m_ptt(); }
  [[nodiscard]] const ReceiverSettings& getRxSettings() const { return m_rxSettings; }
  [[nodiscard]] const TransmitterSettings& getTxSettings() const { return m_txSettings; }

  // void setCentreFrequencyDeltas(int32_t fine, int32_t coarse);
  BandSettings* getFocusBandSettings();
  [[nodiscard]] const BandSettings* getFocusBandSettings() const;
  [[nodiscard]] std::string getFocusBandName() const
  {
    const BandSettings* focusBandSettings = m_activeBandSettings.getFocusBandSettings();

    return focusBandSettings != nullptr ? focusBandSettings->getBandName() : "";
  }

  // void applyRfSettings(const RfSettings& settings, bool onlyChanged = false);
  // void applyIfSettings(const IfSettings& settings);

  bool applyUpdate(SettingUpdate& update) override;
  bool applyUpdate(SettingUpdate& update, BandSelector& bandSelector);

  void clearChanged() override
  {
    SettingsBase::clearChanged();
    m_rxSettings.clearChanged();
    m_txSettings.clearChanged();
  }

  void markAllChanged() override
  {
    SettingsBase::markAllChanged();
    m_rxSettings.markAllChanged();
    m_txSettings.markAllChanged();
    // Not PTT! That being set will short-circuit all other changes
    m_changed &= ~PTT;
  }

  RxPipelineSettings* getFocusPipeline();
  [[nodiscard]] const RxPipelineSettings* getFocusPipeline() const;
  TxPipelineSettings* getTxPipelineSettings();
  [[nodiscard]] const TxPipelineSettings* getTxPipelineSettings() const;
  [[nodiscard]] const Mode* getFocusRxPipelineMode() const;

  static ResultCode getSettingUpdatePath(const SettingPathString& strDottedFeatures, SettingUpdatePath& path);

  static bool getFeaturePath(
    const std::vector<std::string>& featureStrings,
    std::vector<uint32_t>& featuresOut,
    size_t startIndex = 0
    );
protected:
  ActiveBandSettings m_activeBandSettings;
  Setting<bool, PTT, RadioSettings> m_ptt;
  // Setting<std::string, BAND, RadioSettings> m_bandName;
  ReceiverSettings m_rxSettings;
  TransmitterSettings m_txSettings;
};
