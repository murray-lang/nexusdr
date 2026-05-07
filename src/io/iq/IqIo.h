#pragma once
#include "ResultCode.h"
#include "../../core/config-settings/config/audio/IqIoConfig.h"
#include "core/dsp/iq/AudioIqSource.h"
#include "core/dsp/iq/AudioSignalIqSource.h"
#include "core/dsp/iq/IqSource.h"
#include "core/dsp/iq/IqSourceTypes.h"
#include "io/audio/AudioOutputTypes.h"

class IqIo: public AudioSink
{
public:
  IqIo() = default;
  ~IqIo() override = default;

  ResultCode configure(const Config::IqIo::Fields& config);

  void setIqSink(IqSink* pIqSink);
  uint32_t sinkAudio(const RealSamplesMax& samples, uint32_t length, uint32_t numChannels) override;

  [[nodiscard]] uint32_t getInputSampleRate() const;
  [[nodiscard]] uint32_t getOutputSampleRate() const;

  [[nodiscard]] ResultCode start(uint32_t maxFramesPerInputPacket, uint32_t maxFramesPerOutputPacket);
  void stop();

protected:
  ResultCode startInput(uint32_t maxFramesPerInputPacket);
  ResultCode startOutput(uint32_t maxFramesPerOutputPacket);
  void stopInput();
  void stopOutput();

protected:
  IqSourceVariant m_iqSource;
  AudioOutputVariant m_audioOutput;
};
