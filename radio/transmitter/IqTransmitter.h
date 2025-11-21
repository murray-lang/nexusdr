//
// Created by murray on 11/11/25.
//

#pragma once
#include <QObject>

#include "SampleTypes.h"
#include "SignalEmitter.h"
#include "config/TransmitterConfig.h"
#include "../IqPipelineStage.h"
#include "dsp/pipeline/filters/FastFIR.h"
#include "dsp/pipeline/modulators/Modulator.h"
#include "dsp/pipeline/modulators/SsbModulator.h"
#include "dsp/pipeline/oscillators/OscillatorMixer.h"
#include "dsp/pipeline/resampler/Resampler.h"
#include "dsp/utils/HilbertTransform.h"
#include "io/audio/AudioInput.h"
#include "io/audio/AudioOutput.h"
#include "IqTxPipeline.h"
#include "io/iq/IqIo.h"
#include "settings/PttSink.h"
#include "settings/TransmitterSettingsSink.h"


class IqTransmitter : public TransmitterSettingsSink, public PttSink, public SignalEmitter
{
public:
  explicit IqTransmitter(QObject *eventTarget = nullptr);

  ~IqTransmitter() override = default;

  void configure(const TransmitterConfig* pConfig);
  void start() const;
  void stop() const;
  void apply(const TransmitterSettings& settings) override;
  void ptt(bool on) override;

protected:
  IqIo m_iqIo;
  IqTxPipeline m_iqPipeline;
  QObject* m_eventTarget;
};