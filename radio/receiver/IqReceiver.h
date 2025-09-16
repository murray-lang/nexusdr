#ifndef __SDR_H__
#define __SDR_H__

#include <QObject>
#include "../../SampleTypes.h"
#include "../../dsp/stages/oscillators/OscillatorMixer.h"
#include "../../dsp/stages/oscillators/OscillatorInjector.h"
#include "../../dsp/stages/decimator/Decimator.h"
#include "../../dsp/stages/decimator/Decimator.h"
#include "../../dsp/stages/DcShift.h"
#include "../../dsp/stages/filters/FastFIR.h"
#include "../../dsp/utils/PingPongBuffers.h"
#include "../../io/audio/IqSink.h"
#include "../../dsp/stages/demodulators/AmDemodulator.h"
#include "../../dsp/stages/demodulators/FmDemodulator.h"
#include "../../dsp/stages/filters/kernels/BandPassFirKernel.h"
#include "../../SignalEmitter.h"
#include "../../dsp/stages/metering/MeteringStage.h"
#include "../../io/audio/device/AudioOutputDevice.h"
#include "../../io/audio/IqAudioInput.h"
#include "../../io/audio/AudioOutput.h"
#include "config/ReceiverConfig.h"
#include <settings/control/ControlBase.h>
#include <settings/ReceiverSettingsSink.h>
#include <settings/PttSink.h>

//#define PING_PONG_LENGTH 2048
#define PING_PONG_LENGTH 8192

class IqReceiver : public IqSink, public ReceiverSettingsSink, public PttSink, public SignalEmitter {
public:
  explicit IqReceiver( QObject *eventTarget = nullptr);
  // IqReceiver(int32_t sampleRate, size_t defaultFftSize, QObject *eventTarget = nullptr);

  ~IqReceiver() override
  {
    delete m_pIqInput;
  }

  void configure(const ReceiverConfig* pConfig);
  void start() const;
  void stop() const;

  // void applySettings(const RadioSettings& radioSettings) const;
  void apply(const ReceiverSettings& settings) override;

  void ptt(bool on) override;


//  void sink(sdrreal i, sdrreal q) override;
  void sink(ComplexPingPongBuffers& iqBuffers, uint32_t length) override;
  //void processData(const char *data, uint64_t length);

protected:
  std::vector<IqStage*> m_iqStages;
  DcShift m_dcShift;
  OscillatorMixer m_oscillatorMixer;
  // OscillatorInjector m_signal1;
  // OscillatorInjector m_signal2;
  // OscillatorInjector m_signal3;
  Decimator m_decimator;
//  ComplexPingPongBuffers m_ifBuffers;
  RealPingPongBuffers m_afBuffers;
  // Oscillator m_debugOscillator;
  BandPassFilter m_ifFilter;
  BandPassFilter m_afFilter;
  AmDemodulator m_amDemodulator;
  FmDemodulator m_fmDemodulator;
  // MeteringStage m_timeseriesEmitter;
  // MeteringStage m_spectrumEmitter;
  // AudioOutputDevice* m_audioOutput;

  QObject* m_eventTarget;
  IqAudioInput* m_pIqInput;
  AudioOutput* m_pAudioOutput;
  // std::vector<DeviceControl*> m_deviceControllers;
  // ReceiverConfig m_config;
};

#endif //__SDR_H__
