//
// Created by murray on 11/11/25.
//

#pragma once
#include <QObject>

#include "core/SampleTypes.h"
#include "core/config-settings/config/TransmitterConfig.h"
#include "io/audio/AudioInput.h"
#include "io/audio/AudioOutput.h"
#include "core/dsp/pipeline/IqTxPipeline.h"
#include "io/iq/IqIo.h"
#include "io/control/PttSink.h"
#include "core/config-settings/settings/TransmitterSettingsSink.h"
#include "core/config-settings/settings/pipeline/TxPipelineSettings.h"

class ModeSettings;

class IqTransmitter : public TransmitterSettingsSink, public IqSink, public AudioSink, public PttSink
{
public:
  explicit IqTransmitter(QObject *eventTarget = nullptr);

  ~IqTransmitter() override = default;

  void configure(const TransmitterConfig* pConfig);
  void start() const;
  void stop() const;
  void apply(const TransmitterSettings& settings) override;
  void apply(const TxPipelineSettings* settings);
  void ptt(bool on) override;
  void setMode(const Mode& mode);

  uint32_t sinkIq(const ComplexSamplesMax& samples, uint32_t length) override; // IqSink
  uint32_t sinkAudio(const RealSamplesMax& samples, uint32_t length, uint32_t numChannels) override; // AudioSink

  bool adjustRfSettingsToLimits(RfSettings& rfSettings, bool onlyIfChanged = true) const;

protected:
  // const ModeSettings& m_modeSettings;
  IqIo m_iqIo;
  IqTxPipeline m_iqPipeline;
  QObject* m_eventTarget;
};