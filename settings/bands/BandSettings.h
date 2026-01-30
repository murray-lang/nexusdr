//
// Created by murray on 5/1/26.
//

#pragma once

#include "Band.h"
#include "../TxPipelineSettings.h"
#include "../RxPipelineSettings.h"
#include "../base/SettingsBase.h"
#include <QDebug>

#define MAX_RX_PIPELINES 2

class BandSettings : public SettingsBase
{
public:
  enum Features
  {
    NONE = 0,
    WITH_TX_PIPELINE = 0x01,
    WITH_RX_PIPELINE = 0x02,
    FOCUS_RX_PIPELINE = 0x04,
    RX_PIPELINE_TRACKED_BY_TX = 0x08,
    ADD_PIPELINE = 0x10,
    REMOVE_PIPELINE = 0x20,
    ALL = static_cast<uint32_t>(~0U)
  };

  BandSettings():
   m_rxPipelineSettings({}),
   m_focusRxPipeline(this, "focus-rx-pipeline", 0),
   m_rxPipelineTrackedByTx (this, "rx-pipeline-tracked-by-tx", 0)
  {
    BandSettings::setAllChanged();
  }

  BandSettings(const BandSettings& rhs) :
        SettingsBase(rhs),
        m_band(rhs.m_band),
        m_txPipelineSettings(rhs.m_txPipelineSettings),
        m_rxPipelineSettings(rhs.m_rxPipelineSettings),
        m_focusRxPipeline(this, rhs.m_focusRxPipeline),
        m_rxPipelineTrackedByTx(this, rhs.m_rxPipelineTrackedByTx)
  {
  }
  
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
  ~BandSettings() override = default;

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
  [[nodiscard]] uint32_t getNumRxPipelines() const { return m_rxPipelineSettings.size(); }

  // RxPipelineSettings& getRxPipelineSettings(uint32_t index) { return m_rxPipelineSettings[index]; }

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

  [[nodiscard]] bool hasRxFrequencyChanged() const
  {
    const RfSettings& rfSettings = getFocusRxRfSettings();
    return rfSettings.hasSettingChanged(RfSettings::CENTER_FREQUENCY);
  }

  [[nodiscard]] const RxPipelineSettings* getRxPipelineSettings(uint32_t i) const
  {
    if (i < m_rxPipelineSettings.size()) {
      return &m_rxPipelineSettings.at(i);
    }
    return nullptr;
  }

  bool addRxPipeline()
  {
    if (m_rxPipelineSettings.size() >= MAX_RX_PIPELINES) {
      return false;
    }
    m_rxPipelineSettings.emplace_back( );
    RxPipelineSettings& newRxPipeline = m_rxPipelineSettings.back();
    RxPipelineSettings& firstRxPipeline = m_rxPipelineSettings.front();
    newRxPipeline = firstRxPipeline; // (Copy settings)
    if (m_rxPipelineSettings.size() == 1) {
      // The first pipeline is put a little above the centre frequency.
      newRxPipeline.getRfSettings().setVfo(10000);
    } else if (m_rxPipelineSettings.size() == 2) {
      // The second pipeline is a reflection of the first one around the centre frequency.
      const RfSettings& firstRfSettings= firstRxPipeline.getRfSettings();
      int64_t offsetOfFirst = firstRfSettings.getVfo() - firstRfSettings.getCentreFrequency();
      newRxPipeline.getRfSettings().setVfo(firstRfSettings.getCentreFrequency() - offsetOfFirst);
    }
    m_focusRxPipeline(m_rxPipelineSettings.size() - 1); // The new pipeline gets focus
    m_changed |= ADD_PIPELINE;
    return true;
  }

  bool removeRxPipeline(uint32_t index)
  {
    if (index >= m_rxPipelineSettings.size() || m_rxPipelineSettings.size() == 1) {
      return false;
    }
    m_rxPipelineSettings.erase(m_rxPipelineSettings.begin() + index);
    if (m_focusRxPipeline() != 0) {
      m_focusRxPipeline(0);
      m_changed |= FOCUS_RX_PIPELINE;
    }
    m_changed |= REMOVE_PIPELINE;
    return true;
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

  void setCentreFrequencyDeltas(int32_t fine, int32_t coarse)
  {
    m_txPipelineSettings.setCentreFrequencyDeltas(fine, coarse);
    for (RxPipelineSettings& nextSettings : m_rxPipelineSettings) {
      nextSettings.setCentreFrequencyDeltas(fine, coarse);
    }
  }

  void applyRfSettings(const RfSettings& settings, bool onlyChanged = false)
  {
    m_txPipelineSettings.mergeRfSettings(settings, onlyChanged);
    for (RxPipelineSettings& nextSettings : m_rxPipelineSettings) {
      nextSettings.mergeRfSettings(settings, onlyChanged);
    }
  }

  void applyIfSettings(const IfSettings& settings)
  {
    for (RxPipelineSettings& nextSettings : m_rxPipelineSettings) {
      nextSettings.mergeIfSettings(settings);
    }
  }

  bool applyUpdate(SettingUpdate& update) override
  {
    if (update.isExhausted()) {
      throw SettingsException("Invalid setting path");
    }
    uint32_t feature = update.getCurrentFeature();
    const auto& val = update.getValue();

    switch (feature) {
    case FOCUS_RX_PIPELINE:
      return m_focusRxPipeline.apply(val);
    case RX_PIPELINE_TRACKED_BY_TX:
      return applyRxPipelineTrackedByTx(val);
    case WITH_TX_PIPELINE:
      update.stepNextFeature();
      return applyTxPipeline(update);
    case WITH_RX_PIPELINE:
      update.stepNextFeature();
      return applyRxPipeline(update);
    case ADD_PIPELINE:
      return addRxPipeline();
    case REMOVE_PIPELINE:
      return removeRxPipeline(val);
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
    if (key == "with-tx-pipeline") {
      featuresOut.push_back(WITH_TX_PIPELINE);
      return RxPipelineSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
    }
    if (key == "with-rx-pipeline") {
      featuresOut.push_back(WITH_RX_PIPELINE);
      return RxPipelineSettings::getFeaturePath(featureStrings, featuresOut, startIndex + 1);
    }
    if (key == "add-pipeline") {
      featuresOut.push_back(ADD_PIPELINE);
      return true;
    }
    if (key == "remove-pipeline") {
      featuresOut.push_back(REMOVE_PIPELINE);
      return true;
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

  bool applyRxPipeline(SettingUpdate& setting)
  {
    RxPipelineSettings& focusPipelineSettings = m_rxPipelineSettings[m_focusRxPipeline()];
    if (focusPipelineSettings.applyUpdate(setting)) {
      if (m_focusRxPipeline() == m_rxPipelineTrackedByTx()) {
        if (focusPipelineSettings.hasSettingChanged(PipelineSettings::RF)) {
          m_txPipelineSettings.getRfSettings().copyFrequencies(focusPipelineSettings.getRfSettings());
          m_changed |= WITH_TX_PIPELINE;
        }
        if (focusPipelineSettings.hasSettingChanged( PipelineSettings::MODE)) {
          m_txPipelineSettings.setMode(focusPipelineSettings.getMode());
          m_changed |= WITH_TX_PIPELINE;
        }
      }
      m_changed |= WITH_RX_PIPELINE;
      return true;
    }
    return false;
  }

  bool applyTxPipeline(SettingUpdate& setting)
  {
    if (m_txPipelineSettings.applyUpdate(setting)) {
      m_changed |= WITH_TX_PIPELINE;
      return true;
    }
    return false;
  }

  bool applyRxPipelineTrackedByTx(const SettingValue& settingValue)
  {
    uint32_t newRxPipelineIndex = std::get<uint32_t>(settingValue);
    if (m_rxPipelineTrackedByTx() != newRxPipelineIndex) {
      m_rxPipelineTrackedByTx.apply(newRxPipelineIndex);
      RxPipelineSettings& trackedRxPipelineSettings = m_rxPipelineSettings[m_rxPipelineTrackedByTx()];
      m_txPipelineSettings.copyBasicsForTracking(trackedRxPipelineSettings);
      m_changed |= WITH_RX_PIPELINE |WITH_TX_PIPELINE;
      return true;
    }
    return false;
  }

  bool removeRxPipeline(const SettingValue& settingValue)
  {
    uint32_t index = std::get<uint32_t>(settingValue);
    return removeRxPipeline(index);
  }

  Band m_band;
  // const ModeSettings& modeSettings;
  TxPipelineSettings m_txPipelineSettings;
  std::vector<RxPipelineSettings> m_rxPipelineSettings;
  Setting<uint32_t, FOCUS_RX_PIPELINE, BandSettings> m_focusRxPipeline;
  Setting<uint32_t, RX_PIPELINE_TRACKED_BY_TX, BandSettings> m_rxPipelineTrackedByTx;

};