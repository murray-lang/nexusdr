//
// Created by murray on 20/11/25.
//

#pragma once
#include <mutex>

#include "IqPipeline.h"
#include "dsp/utils/HilbertTransform.h"
#include "../../dsp/pipeline/filters/FastFIR.h"
#include "../../dsp/pipeline/modulators/SsbModulator.h"
#include "../../dsp/pipeline/oscillators/OscillatorMixer.h"
#include "../../dsp/pipeline/resampler/Resampler.h"
#include "dsp/pipeline/monitoring/MonitoringStage.h"
#include "settings/TransmitterSettingsSink.h"

class IqTxPipeline: public IqPipeline, public TransmitterSettingsSink
{
public:
  explicit IqTxPipeline(QObject* eventTarget);
  ~IqTxPipeline() override;

  void initialise(IqIo* pIo, AudioSink* pAudioSink) override;
  void setOutputSampleRate(uint32_t outputSampleRate) override;

  uint32_t sinkIq(const vsdrcomplex& samples, uint32_t length) override;

  void ptt(bool on) override {};

  void apply(const TransmitterSettings& settings) override;

  [[nodiscard]] uint32_t getMaxFramesPerInputPacket() const override;
  [[nodiscard]] uint32_t getMaxFramesPerOutputPacket() const override;
  void setMode(const Mode& mode) override;
protected:

  void setModulator(Mode::Type modeType);
  static uint32_t interleaveComplexToReal(const vsdrcomplex& vcomplex, vsdrreal& vreal, uint32_t numComplexes);

protected:
  SsbModulator m_ssbModulator;
  Modulator* m_pModulator;
  Resampler m_resampler;
  OscillatorMixer m_oscillatorMixer;
  BandPassFilter m_ifFilter;
  vsdrreal m_audioBuffer;
  uint32_t m_inputSampleRate;
  uint32_t m_outputSampleRate;
  MonitoringStage* m_pMonitoringStage;
};
