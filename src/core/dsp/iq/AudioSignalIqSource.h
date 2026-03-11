//
// Created by murray on 17/11/25.
//

#pragma once
#include "IqSource.h"
#include "core/config-settings/config/AudioSignalIqSourceConfig.h"
#include "io/audio/AudioInput.h"
#include "io/audio/AudioSink.h"

class AudioSignalIqSource : public IqSource, private AudioSink
{
public:
  AudioSignalIqSource() :
    IqSource(nullptr),
    m_audioInput(dynamic_cast<AudioSink*>(this)),
    m_reverse(false)
  {}
  explicit AudioSignalIqSource(IqSink* pIqSink) :
    IqSource(pIqSink),
    m_audioInput(dynamic_cast<AudioSink*>(this)),
    m_reverse(false)
  {}
  ~AudioSignalIqSource() override = default;

  void configure(const ConfigBase* pConfig) override
  {
    const auto* config = dynamic_cast<const AudioSignalIqSourceConfig*>(pConfig);
    m_audioInput.configure(&config->getAudioInput());
    m_reverse = config->getReverse();
  }

  void start(uint32_t maxPacketFrames) override
  {
    m_audioInput.start(maxPacketFrames);
  }
  void stop() override
  {
    m_audioInput.stop();
  }

  [[nodiscard]] uint32_t getSampleRate() const override { return m_audioInput.getSampleRate(); }

  uint32_t sinkAudio(const vsdrreal& audioSamples, uint32_t length, uint32_t numChannels) override
  {
    uint32_t numFrames = length/2; // Assume numChannels == 2
    if (m_pIqSink != nullptr) {
      m_iqOutputBuffer.resize(length/2);

      if (m_reverse) {
        for (size_t i = 0; i < numFrames; i++) {
          m_iqOutputBuffer.at(i) = sdrcomplex(audioSamples.at(i*2+1), audioSamples.at(i*2));
        }
      } else {
        for (size_t i = 0; i < numFrames; i++) {
          m_iqOutputBuffer.at(i) = sdrcomplex(audioSamples.at(i*2), audioSamples.at(i*2+1));
        }
      }
      m_pIqSink->sinkIq(m_iqOutputBuffer, numFrames);
      return length;
    }
    return 0;
  }

protected:
  AudioInput m_audioInput;
  vsdrcomplex m_iqOutputBuffer;
  bool m_reverse;

};
