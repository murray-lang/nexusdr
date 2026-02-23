//
// Created by murray on 18/11/25.
//

#pragma once
#include <mutex>

#include "correction/DcShift.h"
#include "IqPipeline.h"
#include "agc/IqAgcStage.h"
#include "correction/IqCorrection.h"
#include "decimator/Decimator.h"
#include "demodulators/AmDemodulator.h"
#include "demodulators/Demodulator.h"
#include "demodulators/FmDemodulator.h"
#include "demodulators/SsbDemodulator.h"
#include "demodulators/CwDemodulator.h"
#include "filters/FastFIR.h"
#include "oscillators/OscillatorMixer.h"
#include "config-settings/settings/ReceiverSettingsSink.h"
#include "dsp/pipeline/monitoring/MonitoringStage.h"
#include "SMeter/SMeterStage.h"

class ModeSettings;
class RxPipelineSettings;

class IqRxPipeline : public IqPipeline, public ReceiverSettingsSink
{
public:
  explicit IqRxPipeline(QObject* eventTarget);
  ~IqRxPipeline() override;

  void initialise(IqIo* pIo, AudioSink* pAudioSink) override;
  void setOutputSampleRate(uint32_t outputSampleRate) override;

  uint32_t sinkIq(const vsdrcomplex& samples, uint32_t length) override;

  void ptt(bool on) override {};

  void monitor(bool monitor);

  void apply(const ReceiverSettings& settings) override;

  void apply(const RxPipelineSettings* settings);

  [[nodiscard]] uint32_t getMaxFramesPerInputPacket() const override;
  [[nodiscard]] uint32_t getMaxFramesPerOutputPacket() const override;
  // [[nodiscard]] bool isFrequencyWithinNyquist(int64_t centreFrequency, int64_t frequency) const override;
  void calcNyquistOffsetsLimits(int32_t* maxNegative, int32_t* maxPositive) const override;

  void setMode(const Mode& mode) override;
protected:
  void setDemodulator(const Mode& mode);

protected:
  DcShift m_dcShift;
  IqCorrection m_iqCorrection;
  // OscillatorMixer m_oscillatorMixer;
  Decimator m_decimator;
  BandPassFilter m_ifFilter;

  SMeterStage m_sMeterStage;
  IqAgcStage m_iqAgcStage;

  AmDemodulator m_amDemodulator;
  FmDemodulator m_fmnDemodulator;
  FmDemodulator m_fmwDemodulator;
  SsbDemodulator m_ssbDemodulator;
  CwDemodulator m_cwDemodulator;
  Demodulator* m_pDemodulator;
  vsdrreal m_audioBuffer;
  MonitoringStage* m_pMonitoringStage;
  bool m_monitoring;
};
