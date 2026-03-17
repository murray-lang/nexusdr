//
// Created by murray on 5/08/25.
//

#pragma once

#include "AudioException.h"
#include <AudioDriverFactory.h>
#include "AudioIo.h"
#include "AudioSink.h"
#include "core/config-settings/config/AudioConfig.h"

// template<typename T>
class AudioInput : public AudioIo
{
public:
  explicit AudioInput(AudioSink* pSink) : AudioIo(), m_pDriver(nullptr), m_pSink(pSink)
  {
  }

  ~AudioInput() override
  {
    delete m_pDriver;
  }

  void configure(const AudioConfig* pConfig) override
  {
    // delete m_pDevice;
    m_pDriver = AudioDriverFactory::createInputDriver(pConfig, m_pSink);
  }

  [[nodiscard]] uint32_t getMaxChannels() const 
  {
    if (m_pDriver == nullptr) {
      throw AudioException("IqAudioInput not initialised");
    }

    return m_pDriver->getMaxChannels();
  }

  [[nodiscard]] uint32_t getNumChannels() const 
  {
    if (m_pDriver == nullptr) {
      throw AudioException("IqAudioInput not initialised");
    }

    return m_pDriver->getNumChannels();
  }

  [[nodiscard]] uint32_t getSampleRate() const override
  {
    if (m_pDriver == nullptr) {
      throw AudioException("IqAudioInput not initialised");
    }

    return m_pDriver->getSampleRate();
  }

  void start(uint32_t maxPacketFrames) const override
  {
    if (m_pDriver == nullptr) {
      throw AudioException("IqAudioInput not initialised");
    }
    m_pDriver->start(maxPacketFrames);
  }
  void stop() const override
  {
    if (m_pDriver != nullptr) {
      m_pDriver->stop();
    }
  }
protected:
  AudioInputDriver* m_pDriver;
  AudioSink* m_pSink;
};
