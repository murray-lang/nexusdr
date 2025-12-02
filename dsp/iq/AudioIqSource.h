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
#include <chrono>

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
    // m_lastTime =  std::chrono::steady_clock::now(); 
    m_audioInput.start(maxPacketFrames);
  }
  void stop() override
  {
    m_audioInput.stop();
  }

  [[nodiscard]] uint32_t getSampleRate() const override { return m_audioInput.getSampleRate(); }

  uint32_t sinkAudio(const vsdrreal& audioSamples, uint32_t length, uint32_t numChannels) override
  {
    // auto now = std::chrono::steady_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - m_lastTime).count();
    // m_lastTime = now;
    // qDebug() << "AudioIqSource::sinkAudio(): received" << length << "samples in" << duration << "us";

    if (m_pIqSink != nullptr) {
      uint32_t numFrames  = length / numChannels;
      m_iqOutputBuffer.resize(numFrames);
      // uint32_t outputLength = numFrames;
      uint32_t outputLength = m_hilbert.transform(audioSamples, length, numChannels,m_iqOutputBuffer);
      // for (int i = 0; i < numFrames; ++i) {
      //   size_t audioIndex = i * numChannels;
      //   sdrreal real = audioSamples[audioIndex] * 100; // Take first channel only for now
      //   // Hilbert transform to get imag part
      //   // m_hilbert.transform(vsdrreal(&audioSamples[audioIndex], &audioSamples[audioIndex + 1]), 1, numChannels, m_iqOutputBuffer);
      //   sdrreal imag = 0.0f;
      //   m_iqOutputBuffer[i] = sdrcomplex(real, imag);

      // }
      m_pIqSink->sinkIq(m_iqOutputBuffer, outputLength);
      return length;
    }
    return 0;
  }

protected:
  AudioInput m_audioInput;
  HilbertTransform m_hilbert;
  vsdrcomplex m_iqOutputBuffer;
  // std::chrono::steady_clock::time_point m_lastTime;
};

