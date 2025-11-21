//
// Created by murray on 20/11/25.
//

#pragma once
#include <mutex>

#include "IqPipeline.h"
#include "dsp/utils/HilbertTransform.h"
#include "filters/FastFIR.h"
#include "modulators/SsbModulator.h"
#include "oscillators/OscillatorMixer.h"
#include "resampler/Resampler.h"
#include "settings/TransmitterSettingsSink.h"

class IqTxPipeline: public IqPipeline, public TransmitterSettingsSink
{
public:
  IqTxPipeline();
  ~IqTxPipeline() override = default;

  void initialise(IqIo* pIo, AudioSink* pAudioSink) override;
  void setOutputSampleRate(uint32_t outputSampleRate) override;

  uint32_t sinkIq(const vsdrcomplex& samples, uint32_t length) override;

  void ptt(bool on) override {};

  void apply(const TransmitterSettings& settings) override;

  [[nodiscard]] uint32_t getMaxFramesPerInputPacket() const override;
  [[nodiscard]] uint32_t getMaxFramesPerOutputPacket() const override;

protected:
  void setMode(const Mode& mode) override;
  void setModulator(Mode::Type modeType);

protected:
  SsbModulator m_ssbModulator;
  Modulator* m_pModulator;
  Resampler m_resampler;
  OscillatorMixer m_oscillatorMixer;
  BandPassFilter m_ifFilter;
  vsdrreal m_audioBuffer;
  uint32_t m_inputSampleRate;
  uint32_t m_outputSampleRate;
};
