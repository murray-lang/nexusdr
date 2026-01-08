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
   rxPipelineSettings({}),
   focusRxPipeline(0),
   rxPipelineTrackedByTx (0)
  {
    setAllChanged();
  }

  BandSettings(const Band& band) :
    // modeSettings(),
    m_band(band),
    txPipelineSettings(),
    rxPipelineSettings({}),
    focusRxPipeline(0),
    rxPipelineTrackedByTx (0)
  {
    addRxPipeline();
    applyBandDefaults(band);
    setAllChanged();
  }
  ~BandSettings() override = default;
  // BandSettings(const BandSettings& rhs) = default;

  BandSettings& operator=(const BandSettings& rhs)
  {
    if (this != &rhs) {
      SettingsBase::operator=(rhs);
      m_band = rhs.m_band;
      txPipelineSettings = rhs.txPipelineSettings;
      rxPipelineSettings = rhs.rxPipelineSettings;
      focusRxPipeline = rhs.focusRxPipeline;
      rxPipelineTrackedByTx = rhs.rxPipelineTrackedByTx;
    }
    return *this;
  }

  void clearChanged() override
  {
    SettingsBase::clearChanged();
    txPipelineSettings.clearChanged();
    for (auto& rxSettings : rxPipelineSettings) {
      rxSettings.clearChanged();
    }
  }

  void setAllChanged() override
  {
    SettingsBase::setAllChanged();
    txPipelineSettings.setAllChanged();
    for (auto& rxSettings : rxPipelineSettings) {
      rxSettings.setAllChanged();
    }
  }

  bool hasRxFrequencyChanged() const
  {
    const RfSettings& rfSettings = getFocusRxRfSettings();
    return rfSettings.changed & RfSettings::FREQUENCY;
  }

  void addRxPipeline()
  {
    rxPipelineSettings.push_back({ });
    RxPipelineSettings& newRxPipeline = rxPipelineSettings.back();
    RxPipelineSettings& firstRxPipeline = rxPipelineSettings.front();
    newRxPipeline = firstRxPipeline; // (Copy settings)
    newRxPipeline.rfSettings.offset = 0; // Move the new pipeline to the centre frequency.
    focusRxPipeline = rxPipelineSettings.size() - 1; // The new pipeline gets focus
    changed = RX_PIPELINE | FOCUS_RX_PIPELINE;
  }

  [[nodiscard]] std::string getBandName() const
  {
     return m_band.getName();
  }

  void applyBandDefaults(const Band& band)
  {
    txPipelineSettings.applyBandDefaults(&band);
    for (auto& rxPipeline : rxPipelineSettings) {
      rxPipeline.applyBandDefaults(&band);
    }
  }

  bool setMode(const Mode& mode)
  {
    RxPipelineSettings& focusPipelineSettings = rxPipelineSettings[focusRxPipeline];
    bool focusModeChanged = focusPipelineSettings.setMode(mode);
    if (focusModeChanged && focusRxPipeline == rxPipelineTrackedByTx) {
      txPipelineSettings.setMode(mode);
    }
    return focusModeChanged;
  }

  void applyRfSettings(const RfSettings& settings)
  {
    txPipelineSettings.applyRfSettings(settings);
    for (RxPipelineSettings& nextSettings : rxPipelineSettings) {
      nextSettings.applyRfSettings(settings);
    }
  }

  void applyIfSettings(const IfSettings& settings)
  {
    for (RxPipelineSettings& nextSettings : rxPipelineSettings) {
      nextSettings.applyIfSettings(settings);
    }
  }

  bool applySetting(const SingleSetting& setting, int startIndex) override
  {
    if (startIndex >= setting.getPath().getFeatures().size()) {
      throw SettingsException("Invalid setting path");
    }
    bool settingChange = false;
    uint32_t feature = setting.getPath().getFeatures()[startIndex];

    using Handler = bool (BandSettings::*)(const SingleSetting&, int);
    struct HandlerEntry { Features feature; Handler method; };

    static constexpr HandlerEntry dispatchTable[] = {
      {TX_PIPELINE, &BandSettings::applyTxPipeline },
      {RX_PIPELINE, &BandSettings::applyRxPipeline },
      {FOCUS_RX_PIPELINE, &BandSettings::applyFocusRxPipeline },
      {RX_PIPELINE_TRACKED_BY_TX, &BandSettings::applyRxPipelineTrackedByTx }
    };

    for (const auto& h : dispatchTable) {
      if (feature & h.feature) {
        if ((this->*h.method)(setting, startIndex + 1)) {
          settingChange = true;
          changed |= h.feature;
        }
      }
    }
    return settingChange;
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
    const std::string& key = featureStrings[startIndex];

    using PathFunc = bool(*)(const std::vector<std::string>&, std::vector<uint32_t>&, size_t);
    static const std::map<std::string, std::pair<Features, PathFunc>> dispatch = {
      {"tx-pipeline",{TX_PIPELINE, &TxPipelineSettings::getFeaturePath}},
      {"rx-pipeline",{RX_PIPELINE, &RxPipelineSettings::getFeaturePath}},
      {"focus-rx-pipeline",{FOCUS_RX_PIPELINE, &SettingsBase::addFeature<FOCUS_RX_PIPELINE>}},
      {"rx-pipeline-tracked-by-tx",{RX_PIPELINE_TRACKED_BY_TX, &SettingsBase::addFeature<RX_PIPELINE_TRACKED_BY_TX>}}
    };

    if (auto it = dispatch.find(key); it != dispatch.end()) {
      featuresOut.push_back(it->second.first);
      if (startIndex + 1 < featureStrings.size() && it->second.second) {
        if ( !it->second.second(featureStrings, featuresOut, startIndex + 1)) {
          featuresOut.pop_back();
          return false;
        }
      }
      return true;
    }
    return false;
  }

  bool applyRxPipeline(const SingleSetting& setting, int index)
  {
    RxPipelineSettings& focusPipelineSettings = rxPipelineSettings[focusRxPipeline];
    bool change = focusPipelineSettings.applySetting(setting, index );
    if (change) {
      if (focusRxPipeline == rxPipelineTrackedByTx && focusPipelineSettings.changed & PipelineSettings::RF) {
        txPipelineSettings.rfSettings.copyFrequencies(focusPipelineSettings.rfSettings);
      }
      return true;
    }
    return false;
  }

  //TODO: add/remove rx pipelines.

  RxPipelineSettings* getFocusRxPipelineSettings()
  {
    return &rxPipelineSettings[focusRxPipeline];
  }

  const RxPipelineSettings* getFocusRxPipelineSettings() const
  {
    return &rxPipelineSettings[focusRxPipeline];
  }

  TxPipelineSettings* getTxPipelineSettings()
  {
    return &txPipelineSettings;
  }

  const TxPipelineSettings* getTxPipelineSettings() const
  {
    return &txPipelineSettings;
  }

  RfSettings& getFocusRxRfSettings()
  {
    return rxPipelineSettings[focusRxPipeline].rfSettings;
  }

  const RfSettings& getFocusRxRfSettings() const
  {
    return rxPipelineSettings[focusRxPipeline].rfSettings;
  }

  IfSettings& getFocusRxIfSettings()
  {
    return rxPipelineSettings[focusRxPipeline].ifSettings;
  }

  const IfSettings& getFocusRxIfSettings() const
  {
    return rxPipelineSettings[focusRxPipeline].ifSettings;
  }

  RfSettings& getTxRfSettings()
  {
    return txPipelineSettings.rfSettings;
  }

  const RfSettings& getTxRfSettings() const
  {
    return txPipelineSettings.rfSettings;
  }

  // RxPipelineSettings* getFocusRxPipelineSettings()
  // {
  //   if (focusRxPipeline < rxPipelineSettings.size()) {
  //     return &rxPipelineSettings[focusRxPipeline];
  //   }
  //   return nullptr;
  // }

  // const ModeSettings& modeSettings;
  TxPipelineSettings txPipelineSettings;
  std::vector<RxPipelineSettings> rxPipelineSettings;
  uint32_t focusRxPipeline;
  uint32_t rxPipelineTrackedByTx;
protected:

  bool applyTxPipeline(const SingleSetting& setting, int index)
  {
    return txPipelineSettings.applySetting(setting, index + 1);
  }



  bool applyFocusRxPipeline(const SingleSetting& setting, int index)
  {
    uint32_t newFocusRxPipeline = std::get<uint32_t>(setting.getValue());
    if (focusRxPipeline != newFocusRxPipeline) {
      focusRxPipeline = newFocusRxPipeline;
      return true;
    }
    return false;
  }

  bool applyRxPipelineTrackedByTx(const SingleSetting& setting, int index)
  {
    uint32_t newRxPipelineIndex = std::get<uint32_t>(setting.getValue());
    if (rxPipelineTrackedByTx != newRxPipelineIndex) {
      rxPipelineTrackedByTx = newRxPipelineIndex;
      RxPipelineSettings& trackedRxPipelineSettings = rxPipelineSettings[rxPipelineTrackedByTx];
      txPipelineSettings.copyBasicsForTracking(trackedRxPipelineSettings);
      return true;
    }
    return false;
  }

  Band m_band;
};