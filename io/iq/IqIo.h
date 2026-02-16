//
// Created by murray on 20/11/25.
//

#pragma once
#include "../../config-settings/config/IqIoConfig.h"
#include "dsp/iq/IqSource.h"
#include "io/audio/AudioOutput.h"


class IqIo: public AudioSink
{
public:
  IqIo() : m_pIqSource(nullptr), m_pAudioOutput(nullptr) {}
  virtual ~IqIo()
  {
    delete m_pIqSource;
    delete m_pAudioOutput;
  }

  void configure(const IqIoConfig* pConfig);

  void setIqSink(IqSink* pIqSink);
  uint32_t sinkAudio(const vsdrreal& samples, uint32_t length, uint32_t numChannels) override;

  [[nodiscard]] uint32_t getInputSampleRate() const;
  [[nodiscard]] uint32_t getOutputSampleRate() const;

  void start(uint32_t maxFramesPerInputPacket, uint32_t maxFramesPerOutputPacket) const;
  void stop() const;




protected:
  IqSource* m_pIqSource;
  AudioOutput* m_pAudioOutput;
};
