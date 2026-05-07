//
// Created by murray on 5/08/25.
//

#pragma once

#include <AudioDriverFactory.h>
#include "AudioIo.h"
#include "AudioSink.h"
#include "../../core/config-settings/config/audio/AudioConfig.h"

// template<typename T>
class AudioInput : public AudioIo
{
public:
  explicit AudioInput(AudioSink* pSink) : AudioIo(), m_pSink(pSink)
  {
  }

  ~AudioInput() override = default;

  ResultCode configure(const Config::Audio::Fields& config)
  {
    m_config = config;
    AudioInputDriver* pDriver = nullptr;
    ResultCode rc = AudioDriverFactory::createInputDriver(config, m_pSink, &pDriver);
    if (rc == ResultCode::OK) {
      m_driver.reset(pDriver);
    }
    return rc;
  }

  [[nodiscard]] uint32_t getMaxChannels() const 
  {
    if (!m_driver) {
      return 0;
    }

    return m_driver->getMaxChannels();
  }

  [[nodiscard]] uint32_t getNumChannels() const 
  {
    if (!m_driver) {
      return 0;
    }

    return m_driver->getNumChannels();
  }

  [[nodiscard]] uint32_t getSampleRate() const override
  {
    if (!m_driver) {
      return 0;
    }

    return m_driver->getSampleRate();
  }

  [[nodiscard]] ResultCode start(uint32_t maxPacketFrames) const override
  {
    if (!m_driver ) {
      return ResultCode::ERR_AUDIO_NO_INPUT_DRIVER_CONFIGURED;
    }
    return m_driver->start(maxPacketFrames);
  }

  void stop() const override
  {
    if (m_driver) {
      m_driver->stop();
    }
  }
protected:
  unique_ptr<AudioInputDriver> m_driver;
  AudioSink* m_pSink;
};
