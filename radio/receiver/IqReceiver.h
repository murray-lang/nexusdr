#pragma once

#include "../../SignalEmitter.h"
#include "../../io/audio/drivers/RtAudio/RtAudioOutputDriver.h"
#include "config/ReceiverConfig.h"
#include <settings/ReceiverSettingsSink.h>
#include "../../dsp/pipeline/IqRxPipeline.h"
#include "io/iq/IqIo.h"

class RxPipelineSettings;

//#define PING_PONG_LENGTH 2048
// #define PING_PONG_LENGTH 8192

class ModeSettings;

class IqReceiver : public ReceiverSettingsSink, public IqSink, public AudioSink, public PttSink, public SignalEmitter {
public:
  explicit IqReceiver(QObject *eventTarget = nullptr);

  ~IqReceiver() override = default;

  void configure(const ReceiverConfig* pConfig);

  uint32_t sinkIq(const vsdrcomplex& samples, uint32_t length) override; // IqSink
  uint32_t sinkAudio(const vsdrreal& samples, uint32_t length, uint32_t numChannels) override; // AudioSink

  void start();
  void stop();

  void apply(const ReceiverSettings& settings) override;
  void apply(const RxPipelineSettings* settings);

  void ptt(bool on) override;

  void setMode(const Mode& mode);

  bool adjustRfSettingsToLimits(RfSettings& rfSettings, bool onlyIfChanged = true) const;

protected:
  // const ModeSettings& m_modeSettings;
  IqIo m_iqIo;
  IqRxPipeline m_iqPipeline;
  QObject* m_eventTarget;
};

