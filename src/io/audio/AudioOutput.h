//
// Created by murray on 5/08/25.
//

#pragma once


#include "AudioIo.h"
#include "drivers/RtAudio/RtAudioOutputDriver.h"
#include <AudioDriverFactory.h>

class AudioOutput : public AudioIo, public AudioSink
{
  public:
  AudioOutput() : AudioIo()
  {
  }

  ~AudioOutput() override = default;

  ResultCode configure(const Config::Audio::Fields& config)
  {
    m_config = config;
    AudioOutputDriver* pDriver = nullptr;
    ResultCode rc = AudioDriverFactory::createOutputDriver(config , &pDriver);
    if (rc == ResultCode::OK) {
      m_pDriver.reset(pDriver);
    }
    return rc;
  }

  [[nodiscard]] ResultCode start(uint32_t maxPacketFrames) const override
  {
    if (!m_pDriver) {
      return ResultCode::ERR_AUDIO_NO_OUTPUT_DRIVER_CONFIGURED;
    }
    return m_pDriver->start(maxPacketFrames);
    // m_pSink->setVolume(1.0); How to do this with RtAudio???
  }

  void stop() const override
  {
    if (m_pDriver) {
      m_pDriver->stop();
    }
  }

  [[nodiscard]] uint32_t getSampleRate() const override
  {
    if (!m_pDriver) {
      return 0;
    }

    return m_pDriver->getSampleRate();
  }

  uint32_t sinkAudio(const RealSamplesMax& data, uint32_t length, uint32_t numChannels) override
  {
    if (!m_pDriver)
    {
      return 0;
      // throw AudioException("AudioOutput not initialised");
    }
    return m_pDriver->addAudioData(data, length, numChannels);
  }
protected:
  unique_ptr<AudioOutputDriver> m_pDriver;

};

