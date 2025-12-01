//
// Created by murray on 11/11/25.
//

#pragma once
#include <QObject>

#include "SampleTypes.h"
#include "SignalEmitter.h"
#include "config/TransmitterConfig.h"
#include "io/audio/AudioInput.h"
#include "io/audio/AudioOutput.h"
#include "../../dsp/pipeline/IqTxPipeline.h"
#include "io/iq/IqIo.h"
#include "settings/PttSink.h"
#include "settings/TransmitterSettingsSink.h"

class ModeSettings;

class IqTransmitter : public TransmitterSettingsSink, public IqSink, public AudioSink, public PttSink, public SignalEmitter
{
public:
  explicit IqTransmitter(const ModeSettings& modeSettings, QObject *eventTarget = nullptr);

  ~IqTransmitter() override = default;

  void configure(const TransmitterConfig* pConfig);
  void start() const;
  void stop() const;
  void apply(const TransmitterSettings& settings) override;
  void ptt(bool on) override;
  void setMode(const Mode& mode);

  uint32_t sinkIq(const vsdrcomplex& samples, uint32_t length) override; // IqSink
  uint32_t sinkAudio(const vsdrreal& samples, uint32_t length, uint32_t numChannels) override; // AudioSink

protected:
  const ModeSettings& m_modeSettings;
  IqIo m_iqIo;
  IqTxPipeline m_iqPipeline;
  QObject* m_eventTarget;
};