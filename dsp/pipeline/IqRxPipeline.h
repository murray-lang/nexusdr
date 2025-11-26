//
// Created by murray on 18/11/25.
//

#pragma once
#include <mutex>

#include "DcShift.h"
#include "IqPipeline.h"
#include "decimator/Decimator.h"
#include "demodulators/AmDemodulator.h"
#include "demodulators/Demodulator.h"
#include "demodulators/FmDemodulator.h"
#include "demodulators/SsbDemodulator.h"
#include "demodulators/CwDemodulator.h"
#include "filters/FastFIR.h"
#include "oscillators/OscillatorMixer.h"
#include "settings/ReceiverSettingsSink.h"
#include "dsp/pipeline/monitoring/MonitoringStage.h"

class ModeSettings;

class IqRxPipeline : public IqPipeline, public ReceiverSettingsSink
{
public:
  explicit IqRxPipeline(const ModeSettings& modeSettings, QObject* eventTarget);
  ~IqRxPipeline() override = default;

  void initialise(IqIo* pIo, AudioSink* pAudioSink) override;
  void setOutputSampleRate(uint32_t outputSampleRate) override;

  uint32_t sinkIq(const vsdrcomplex& samples, uint32_t length) override;

  void ptt(bool on) override {};

  void apply(const ReceiverSettings& settings) override;

  [[nodiscard]] uint32_t getMaxFramesPerInputPacket() const override;
  [[nodiscard]] uint32_t getMaxFramesPerOutputPacket() const override;

  void setMode(const Mode& mode) override;
protected:
  void setDemodulator(const Mode& mode);

protected:
  DcShift m_dcShift;
  OscillatorMixer m_oscillatorMixer;
  Decimator m_decimator;
  BandPassFilter m_ifFilter;
  AmDemodulator m_amDemodulator;
  FmDemodulator m_fmDemodulator;
  SsbDemodulator m_ssbDemodulator;
  CwDemodulator m_cwDemodulator;
  Demodulator* m_pDemodulator;
  vsdrreal m_audioBuffer;
  MonitoringStage* m_pMonitoringStage;
};
