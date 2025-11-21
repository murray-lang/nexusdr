#pragma once

#include "../../SignalEmitter.h"
#include "../../io/audio/device/AudioOutputDevice.h"
#include "config/ReceiverConfig.h"
#include <settings/ReceiverSettingsSink.h>
#include <settings/PttSink.h>
#include "dsp/pipeline/IqRxPipeline.h"
#include "io/iq/IqIo.h"


//#define PING_PONG_LENGTH 2048
// #define PING_PONG_LENGTH 8192

class IqReceiver : public ReceiverSettingsSink, public IqSink, public AudioSink, public PttSink, public SignalEmitter {
public:
  explicit IqReceiver(QObject *eventTarget = nullptr);

  ~IqReceiver() override = default;

  void configure(const ReceiverConfig* pConfig);

  uint32_t sinkIq(const vsdrcomplex& samples, uint32_t length) override; // IqSink
  uint32_t sinkAudio(const vsdrreal& samples, uint32_t length) override; // AudioSink

  void start();
  void stop();

  void apply(const ReceiverSettings& settings) override;

  void ptt(bool on) override;

protected:
  IqIo m_iqIo;
  IqRxPipeline m_iqPipeline;
  QObject* m_eventTarget;
};

