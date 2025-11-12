//
// Created by murray on 11/11/25.
//

#ifndef CUTESDR_VK6HL_IQTRANSMITTER_H
#define CUTESDR_VK6HL_IQTRANSMITTER_H
#include <QObject>

#include "SampleTypes.h"
#include "SignalEmitter.h"
#include "config/TransmitterConfig.h"
#include "dsp/stages/IqStage.h"
#include "dsp/stages/filters/FastFIR.h"
#include "dsp/stages/oscillators/OscillatorMixer.h"
#include "io/audio/AudioInput.h"
#include "io/audio/AudioOutput.h"
#include "io/audio/AudioSink.h"
#include "settings/PttSink.h"
#include "settings/TransmitterSettingsSink.h"


class IqTransmitter : public AudioSink<sdrreal>, public TransmitterSettingsSink, public PttSink, public SignalEmitter
{
public:
  explicit IqTransmitter(QObject *eventTarget = nullptr);

  ~IqTransmitter() override
  {
    delete m_pAudioInput;
    delete m_pIqOutput;
  }

  void configure(const TransmitterConfig* pConfig);
  void start() const;
  void stop() const;
  void apply(const TransmitterSettings& settings) override;
  void ptt(bool on) override;

  void sink(RealPingPongBuffers& audioBuffers, uint32_t length) override;


protected:
  Mode m_mode;
  std::vector<IqStage*> m_iqStages;
  OscillatorMixer m_oscillatorMixer;
  RealPingPongBuffers m_afBuffers;
  // Oscillator m_debugOscillator;
  BandPassFilter m_ifFilter;
  BandPassFilter m_afFilter;
  // AmDemodulator m_amDemodulator;
  // FmDemodulator m_fmDemodulator;
  // SsbDemodulator m_ssbDemodulator;
  // Demodulator* m_pDemodulator;
  // std::mutex m_demodulatorMutex;
  // MeteringStage m_timeseriesEmitter;
  // MeteringStage m_spectrumEmitter;
  // AudioOutputDevice* m_audioOutput;

  QObject* m_eventTarget;
  AudioInput<sdrreal>* m_pAudioInput;
  AudioOutput* m_pIqOutput;
};


#endif //CUTESDR_VK6HL_IQTRANSMITTER_H