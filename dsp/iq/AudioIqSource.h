//
// Created by murray on 17/11/25.
//

#pragma once
#include "IqSource.h"
#include "SampleTypes.h"
#include "config/AudioIqSourceConfig.h"
#include "dsp/utils/HilbertTransform.h"
#include "io/audio/AudioInput.h"
#include "qdebug.h"

class AudioIqSource : public IqSource, private AudioSink
{
public:
  AudioIqSource() :
    IqSource(nullptr),
    m_audioInput(dynamic_cast<AudioSink*>(this)),
    m_hilbert(63)
  {
  };

  explicit AudioIqSource(IqSink* pIqSink) :
    IqSource(pIqSink),
    m_audioInput(dynamic_cast<AudioSink*>(this)),
    m_hilbert(63)
  {
  };
  ~AudioIqSource() override = default;


  void configure(const ConfigBase* pConfig) override
  {
    const auto * config = dynamic_cast<const AudioIqSourceConfig*>(pConfig);
    m_audioInput.configure(&config->getAudioInput());
    qDebug() << "AudioIqSource max channels:" << m_audioInput.getMaxChannels() << ", num channels:" << m_audioInput.getNumChannels();
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

  uint32_t sinkAudio(const vsdrreal& audioSamples, uint32_t length) override
  {
    if (m_pIqSink != nullptr) {
      uint32_t numFrames  = length / m_audioInput.getNumChannels();
      m_iqOutputBuffer.resize(numFrames);
      uint32_t outputLength = m_hilbert.transform(audioSamples, m_iqOutputBuffer, length);
      m_pIqSink->sinkIq(m_iqOutputBuffer, outputLength);
      return length;
    }
    return 0;
  }

protected:
  AudioInput m_audioInput;
  HilbertTransform m_hilbert;
  vsdrcomplex m_iqOutputBuffer;
};

