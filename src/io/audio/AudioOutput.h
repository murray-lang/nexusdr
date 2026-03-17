//
// Created by murray on 5/08/25.
//

#pragma once


#include "AudioException.h"
#include "AudioIo.h"
#include "drivers/RtAudio/RtAudioOutputDriver.h"
#include <AudioDriverFactory.h>

class AudioOutput : public AudioIo, public AudioSink
{
  public:
  AudioOutput() : AudioIo(), m_pDriver(nullptr)
  {
  }

  ~AudioOutput() override
  {
    delete m_pDriver;
  }

  void configure(const AudioConfig* pConfig) override
  {
    delete m_pDriver;
    m_pDriver = AudioDriverFactory::createOutputDriver(pConfig);
  }

  void start(uint32_t maxPacketFrames) const override
  {
    if (m_pDriver == nullptr)
    {
      throw AudioException("AudioOutput not initialised");
    }
    m_pDriver->start(maxPacketFrames);
    // m_pSink->setVolume(1.0); How to do this with RtAudio???
  }

  void stop() const override
  {
    if (m_pDriver != nullptr) {
      m_pDriver->stop();
    }
  }

  [[nodiscard]] uint32_t getSampleRate() const override
  {
    if (m_pDriver == nullptr) {
      throw AudioException("IqAudioOutput not initialised");
    }

    return m_pDriver->getSampleRate();
  }

  uint32_t sinkAudio(const vsdrreal& data, uint32_t length, uint32_t numChannels) override
  {
    if (m_pDriver == nullptr)
    {
      return 0;
      // throw AudioException("AudioOutput not initialised");
    }
    return m_pDriver->addAudioData(data, length, numChannels);
  }
protected:
  AudioOutputDriver* m_pDriver;

};

