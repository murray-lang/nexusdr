//
// Created by murray on 5/1/26.
//

#pragma once

#include "Band.h"
#include "TxPipelineSettings.h"
#include "RxPipelineSettings.h"
#include "SettingsBase.h"
#include <QDebug>


class BandSettings : public SettingsBase
{
public:
  enum Features
  {
    NONE = 0,
    TX_PIPELINE = 0x01,
    RX_PIPELINE = 0x02,
    FOCUS_RX_PIPELINE = 0x04,
    RX_PIPELINE_TRACKED_BY_TX = 0x08,
    ALL = static_cast<uint32_t>(~0U)
  };

  BandSettings():
   m_rxPipelineSettings({}),
   m_focusRxPipeline(this, "focus-rx-pipeline", 0),
   m_rxPipelineTrackedByTx (this, "rx-pipeline-tracked-by-tx", 0)
  {
    BandSettings::setAllChanged();
  }

  BandSettings(const BandSettings& rhs) = default;
  
  BandSettings(const Band& band) :
    // modeSettings(),
    m_band(band),
    m_txPipelineSettings(),
    m_rxPipelineSettings({}),
    m_focusRxPipeline(this, "focus-rx-pipeline", 0),
    m_rxPipelineTrackedByTx (this, "rx-pipeline-tracked-by-tx", 0)
  {
    addRxPipeline();
    applyBandDefaults(band);
    BandSettings::setAllChanged();
  }
  ~BandSettings() override 
  {}
  // BandSettings(const BandSettings& rhs) = default;

  BandSettings& operator=(const BandSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      m_band = rhs.m_band;
      m_txPipelineSettings = rhs.m_txPipelineSettings;
      m_rxPipelineSettings = rhs.m_rxPipelineSettings;
      m_focusRxPipeline = rhs.m_focusRxPipeline;
      m_rxPipelineTrackedByTx = rhs.m_rxPipelineTrackedByTx;
    }
    return *this;
  }

  [[nodiscard]] const Band& getBand() const { return m_band; }
  [[nodiscard]] uint32_t getFocusRxPipeline() const { return m_focusRxPipeline(); }
  [[nodiscard]] uint32_t getRxPipelineTrackedByTx() const { return m_rxPipelineTrackedByTx(); }

  void clearChanged() override
  {
    SettingsBase::clearChanged();
    m_txPipelineSettings.clearChanged();
    for (auto& rxSettings : m_rxPipelineSettings) {
      rxSettings.clearChanged();
    }
  }

  void setAllChanged() override
  {
    SettingsBase::setAllChanged();
    m_txPipelineSettings.setAllChanged();
    for (auto& rxSettings : m_rxPipelineSettings) {
      rxSettings.setAllChanged();
    }
  }

  bool hasRxFrequencyChanged() const
  {
    const RfSettings& rfSettings = getFocusRxRfSettings();
    return rfSettings.hasSettingChanged(RfSettings::FREQUENCY);
  }

  void addRxPipeline()
  {
    m_rxPipelineSettings.emplace_back( );
    RxPipelineSettings& newRxPipeline = m_rxPipelineSettings.back();
    RxPipelineSettings& firstRxPipeline = m_rxPipelineSettings.front();
    newRxPipeline = firstRxPipeline; // (Copy settings)
    newRxPipeline.getRfSettings().setOffset(10000); // Move the new pipeline to the centre frequency.
    m_focusRxPipeline(m_rxPipelineSettings.size() - 1); // The new pipeline gets focus
    m_changed |= RX_PIPELINE;
  }

  [[nodiscard]] std::string getBandName() const
  {
     return m_band.getName();
  }

  void applyBandDefaults(const Band& band)
  {
    m_txPipelineSettings.applyBandDefaults(&band);
    for (auto& rxPipeline : m_rxPipelineSettings) {
      rxPipeline.applyBandDefaults(&band);
    }
  }

  bool setMode(const Mode& mode)
  {
    RxPipelineSettings& focusPipelineSettings = m_rxPipelineSettings[m_focusRxPipeline()];
    bool focusModeChanged = focusPipelineSettings.setMode(mode);
    if (focusModeChanged && m_focusRxPipeline() == m_rxPipelineTrackedByTx()) {
      m_txPipelineSettings.setMode(mode);
    }
    return focusModeChanged;
  }

  void applyRfSettings(const RfSettings& settings)
  {
    m_txPipelineSettings.mergeRfSettings(settings);
    for (RxPipelineSettings& nextSettings : m_rxPipelineSettings) {
      nextSettings.mergeRfSettings(settings);
    }
  }

  void applyIfSettings(const IfSettings& settings)
  {
    for (RxPipelineSettings& nextSettings : m_rxPipelineSettings) {
      nextSettings.mergeIfSettings(settings);
    }
  }

  bool applyUpdate(const SettingUpdate& update, int startIndex) override
  {
    const auto& features = update.getPath().getFeatures();
    if (startIndex >= features.size()) {
      throw SettingsException("Invalid setting path");
    }
    uint32_t feature = features[startIndex];
    const auto& val = update.getValue();

    switch (feature) {
    case FOCUS_RX_PIPELINE:
      return m_focusRxPipeline.apply(val);
    case RX_PIPELINE_TRACKED_BY_TX:
      return applyRxPipelineTrackedByTx(update, startIndex);
    case TX_PIPELINE:
      return applyTxPipeline(update, startIndex + 1);
    case RX_PIPELINE:
      return applyRxPipeline(update, startIndex + 1);
    default:
      return false;
    }
  }

  static bool getFeaturePath(
    const std::vector<std::string>& featureStrings,
    std::vector<uint32_t>& featuresOut,
    size_t startIndex
    )
  {
    if (startIndex >= featureStrings.size()) {
      throw SettingsException("Invalid feature path");
    }
    if (resolvePathForRegisteredSetting<BandSettings>(featureStrings, featuresOut, startIndex)) {
      return true;
    }
    const std::string& key = featureStrings[startIndex];
    if (key == "tx-pipeline") {
      featuresOut.push_back(TX_PIPELINE);
      return RxPipelineSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
    } else if (key == "rx-pipeline") {
      featuresOut.push_back(RX_PIPELINE);
      return RxPipelineSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
    }
    return false;
  }

  //TODO: add/remove rx pipelines.

  RxPipelineSettings* getFocusRxPipelineSettings()
  {
    return &m_rxPipelineSettings[m_focusRxPipeline()];
  }

  [[nodiscard]] const RxPipelineSettings* getFocusRxPipelineSettings() const
  {
    return &m_rxPipelineSettings[m_focusRxPipeline()];
  }

  [[nodiscard]] const Mode& getFocusRxPipelineMode() const
  {
    return m_rxPipelineSettings[m_focusRxPipeline()].getMode();
  }

  TxPipelineSettings* getTxPipelineSettings()
  {
    return &m_txPipelineSettings;
  }

  [[nodiscard]] const TxPipelineSettings* getTxPipelineSettings() const
  {
    return &m_txPipelineSettings;
  }

  RfSettings& getFocusRxRfSettings()
  {
    return m_rxPipelineSettings[m_focusRxPipeline()].getRfSettings();
  }

  [[nodiscard]] const RfSettings& getFocusRxRfSettings() const
  {
    return m_rxPipelineSettings[m_focusRxPipeline()].getRfSettings();
  }

  IfSettings& getFocusRxIfSettings()
  {
    return m_rxPipelineSettings[m_focusRxPipeline()].getIfSettings();
  }

  [[nodiscard]] const IfSettings& getFocusRxIfSettings() const
  {
    return m_rxPipelineSettings[m_focusRxPipeline()].getIfSettings();
  }

  RfSettings& getTxRfSettings()
  {
    return m_txPipelineSettings.getRfSettings();
  }

  [[nodiscard]] const RfSettings& getTxRfSettings() const
  {
    return m_txPipelineSettings.getRfSettings();
  }

  // RxPipelineSettings* getFocusRxPipelineSettings()
  // {
  //   if (focusRxPipeline < rxPipelineSettings.size()) {
  //     return &rxPipelineSettings[focusRxPipeline];
  //   }
  //   return nullptr;
  // }

protected:

  bool applyRxPipeline(const SettingUpdate& setting, int index)
  {
    RxPipelineSettings& focusPipelineSettings = m_rxPipelineSettings[m_focusRxPipeline()];
    if (focusPipelineSettings.applyUpdate(setting, index)) {
      if (m_focusRxPipeline() == m_rxPipelineTrackedByTx()) {
        if (focusPipelineSettings.hasSettingChanged(PipelineSettings::RF)) {
          m_txPipelineSettings.getRfSettings().copyFrequencies(focusPipelineSettings.getRfSettings());
          m_changed |= TX_PIPELINE;
        }
        if (focusPipelineSettings.hasSettingChanged( PipelineSettings::MODE)) {
          m_txPipelineSettings.setMode(focusPipelineSettings.getMode());
          m_changed |= TX_PIPELINE;
        }
      }
      m_changed |= RX_PIPELINE;
      return true;
    }
    return false;
  }

  bool applyTxPipeline(const SettingUpdate& setting, int index)
  {
    if (m_txPipelineSettings.applyUpdate(setting, index)) {
      m_changed |= TX_PIPELINE;
      return true;
    }
    return false;
  }

  bool applyRxPipelineTrackedByTx(const SettingUpdate& setting, int index)
  {
    uint32_t newRxPipelineIndex = std::get<uint32_t>(setting.getValue());
    if (m_rxPipelineTrackedByTx() != newRxPipelineIndex) {
      m_rxPipelineTrackedByTx.apply(newRxPipelineIndex);
      RxPipelineSettings& trackedRxPipelineSettings = m_rxPipelineSettings[m_rxPipelineTrackedByTx()];
      m_txPipelineSettings.copyBasicsForTracking(trackedRxPipelineSettings);
      m_changed |= RX_PIPELINE |TX_PIPELINE;
      return true;
    }
    return false;
  }

  Band m_band;
  // const ModeSettings& modeSettings;
  TxPipelineSettings m_txPipelineSettings;
  std::vector<RxPipelineSettings> m_rxPipelineSettings;
  Setting<uint32_t, FOCUS_RX_PIPELINE, BandSettings> m_focusRxPipeline;
  Setting<uint32_t, RX_PIPELINE_TRACKED_BY_TX, BandSettings> m_rxPipelineTrackedByTx;

};