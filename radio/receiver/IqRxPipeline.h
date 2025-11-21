//
// Created by murray on 18/11/25.
//

#pragma once
#include <mutex>

#include "../../dsp/pipeline/DcShift.h"
#include "../IqPipeline.h"
#include "../../dsp/pipeline/decimator/Decimator.h"
#include "../../dsp/pipeline/demodulators/AmDemodulator.h"
#include "../../dsp/pipeline/demodulators/Demodulator.h"
#include "../../dsp/pipeline/demodulators/FmDemodulator.h"
#include "../../dsp/pipeline/demodulators/SsbDemodulator.h"
#include "../../dsp/pipeline/filters/FastFIR.h"
#include "../../dsp/pipeline/oscillators/OscillatorMixer.h"
#include "settings/ReceiverSettingsSink.h"



class IqRxPipeline : public IqPipeline, public ReceiverSettingsSink
{
public:
  IqRxPipeline();
  ~IqRxPipeline() override = default;

  void initialise(IqIo* pIo, AudioSink* pAudioSink) override;
  void setOutputSampleRate(uint32_t outputSampleRate) override;

  uint32_t sinkIq(const vsdrcomplex& samples, uint32_t length) override;

  void ptt(bool on) override {};

  void apply(const ReceiverSettings& settings) override;

  [[nodiscard]] uint32_t getMaxFramesPerInputPacket() const override;
  [[nodiscard]] uint32_t getMaxFramesPerOutputPacket() const override;

protected:
  void setMode(const Mode& mode) override;
  void setDemodulator(Mode::Type modeType);

protected:
  DcShift m_dcShift;
  OscillatorMixer m_oscillatorMixer;
  Decimator m_decimator;
  BandPassFilter m_ifFilter;
  AmDemodulator m_amDemodulator;
  FmDemodulator m_fmDemodulator;
  SsbDemodulator m_ssbDemodulator;
  Demodulator* m_pDemodulator;
  vsdrreal m_audioBuffer;
};
