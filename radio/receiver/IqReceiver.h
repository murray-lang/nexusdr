#pragma once

#include "../../SignalEmitter.h"
#include "../../io/audio/drivers/RtAudio/RtAudioOutputDriver.h"
#include "../../config-settings/config/ReceiverConfig.h"
#include "config-settings/settings/ReceiverSettingsSink.h"
#include "../../dsp/pipeline/IqRxPipeline.h"
#include "dsp/pipeline/RxPipelineAudioTap.h"
#include "io/iq/IqIo.h"

class BandSettings;
class RxPipelineSettings;
class ModeSettings;

class IqReceiver :
  public ReceiverSettingsSink,
  public IqSink,
  public PttSink,
  public SignalEmitter,
  public RxPipelineAudioRouter
{
public:
  explicit IqReceiver(QObject *eventTarget = nullptr);

  ~IqReceiver() override = default;

  void configure(const ReceiverConfig* pConfig);

  uint32_t sinkIq(const vsdrcomplex& samples, uint32_t length) override; // IqSink

  // Called by RxPipelineAudioTap (router interface)
  uint32_t sinkPipelineAudio(PipelineId pipelineId,
                             const vsdrreal& samples,
                             uint32_t length,
                             uint32_t numChannels) override;

  void start();
  void stop();

  void apply(const ReceiverSettings& settings) override;
  // void apply(const RxPipelineSettings* settings);
  void apply(BandSettings* settings);

  void ptt(bool on) override;

  void setMode(const Mode& mode);

  bool adjustRfSettingsToLimits(RxPipelineSettings* rxPipelineSettings, IqRxPipeline& pipeline, bool onlyIfChanged = true) const;

protected:
  static uint32_t downmixToMono(const vsdrreal& in,
                               uint32_t length,
                               uint32_t numChannels,
                               vsdrreal& outMono);

  void outputStereoFromMono(const vsdrreal& leftMono,
                            const vsdrreal& rightMono,
                            uint32_t frames);

  void outputStereoDuplicate(const vsdrreal& mono, uint32_t frames);

protected:
  // const ModeSettings& m_modeSettings;
  IqIo m_iqIo;
  std::string m_bandName;
  IqRxPipeline m_iqPipelineA;
  IqRxPipeline m_iqPipelineB;
  PipelineId m_focusPipelineId;
  QObject* m_eventTarget;

  RxPipelineAudioTap m_audioTapA;
  RxPipelineAudioTap m_audioTapB;

  bool m_pipelineBEnabled;

  bool m_havePendingA;
  bool m_havePendingB;
  vsdrreal m_pendingA;
  vsdrreal m_pendingB;

  vsdrreal m_stereoOutInterleaved;
};

