//
// Created by murray on 5/1/26.
//

#pragma once

#include "Band.h"
#include "../pipeline/TxPipelineSettings.h"
#include "../pipeline/RxPipelineSettings.h"
#include "../base/SettingsBase.h"
#include <QDebug>

#define MAX_PIPELINES 2

class BandSettings : public SettingsBase
{
public:
  enum Features
  {
    NONE = 0,
    MULTI_PIPELINE = 0x01,
    FOCUS_PIPELINE   = 0x02,
    WITH_FOCUS_PIPELINE = 0x04,
    TX_PIPELINE  = 0x08,
    WITH_TX_PIPELINE   = 0x10,
    CLOSE_PIPELINE = 0x20,
    ALL = static_cast<uint32_t>(~0U)
  };

  BandSettings();

  BandSettings(const BandSettings& rhs);
  
  explicit BandSettings(const Band& band);
  ~BandSettings() override = default;

  BandSettings& operator=(const BandSettings& rhs);

  [[nodiscard]] const Band& getBand() const { return m_band; }
  [[nodiscard]] bool isMultiPipeline() const { return m_isMultiPipeline(); }
  [[nodiscard]] PipelineId getTxPipelineId() const { return m_txPipelineId(); }

  bool splitPipelines(bool split);
  [[nodiscard]] PipelineId getFocusPipelineId() const { return m_focusPipelineId(); }
  RxPipelineSettings* getFocusPipeline();
  [[nodiscard]] const RxPipelineSettings* getFocusPipeline() const;
  TxPipelineSettings* getTxPipeline();
  [[nodiscard]] const TxPipelineSettings* getTxPipeline() const;
  RxPipelineSettings* getRxPipeline(PipelineId id);
  [[nodiscard]] const RxPipelineSettings* getRxPipeline(PipelineId id) const;

  RfSettings* getFocusRxRfSettings();
  [[nodiscard]] const RfSettings* getFocusRxRfSettings() const;
  IfSettings* getFocusRxIfSettings();
  [[nodiscard]] const IfSettings* getFocusRxIfSettings() const;
  [[nodiscard]] const Mode* getFocusRxMode() const;

  bool closePipeline(PipelineId id);
  void clearChanged() override;
  void setAllChanged() override;

  [[nodiscard]] std::string getBandName() const;

  void applyBandDefaults(const Band& band);

  bool setMode(const Mode& mode);

  void setCentreFrequencyDeltas(int32_t fine, int32_t coarse);

  void applyRfSettings(const RfSettings& settings, bool onlyChanged = false);

  void applyIfSettings(const IfSettings& settings);

  bool applyUpdate(SettingUpdate& update) override;

  static bool getFeaturePath(
    const std::vector<std::string>& featureStrings,
    std::vector<uint32_t>& featuresOut,
    size_t startIndex
    );

  RfSettings& getTxRfSettings();

  [[nodiscard]] const RfSettings& getTxRfSettings() const;

protected:

  bool applySplitPipelines(const SettingValue& settingValue);
  bool setTxPipeline(const SettingValue& settingValue);
  bool setFocusPipeline(const SettingValue& settingValue);
  bool applyClosePipeline(const SettingValue& settingValue);
  bool withFocusPipeline(SettingUpdate& setting);
  bool withTxPipeline(SettingUpdate& setting);

  Band m_band;
  RxPipelineSettings m_pipelineA;
  std::optional<RxPipelineSettings> m_pipelineB;
  TxPipelineSettings m_txPipeline;

  Setting<bool, MULTI_PIPELINE, BandSettings> m_isMultiPipeline;
  Setting<PipelineId, FOCUS_PIPELINE, BandSettings> m_focusPipelineId;
  Setting<PipelineId, TX_PIPELINE, BandSettings> m_txPipelineId;

};