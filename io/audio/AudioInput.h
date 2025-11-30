//
// Created by murray on 5/08/25.
//

#pragma once

#include "AudioException.h"
#include "./device/AudioDeviceFactory.h"
#include "AudioIo.h"
#include "AudioSink.h"
#include "config/AudioConfig.h"

// template<typename T>
class AudioInput : public AudioIo
{
public:
  explicit AudioInput(AudioSink* pSink) : AudioIo(), m_pDevice(nullptr), m_pSink(pSink)
  {
  }

  ~AudioInput() override
  {
    delete m_pDevice;
  }

  void configure(const AudioConfig* pConfig) override
  {
    // delete m_pDevice;
    m_pDevice = AudioDeviceFactory::createInputDevice(pConfig, m_pSink);
  }

  [[nodiscard]] uint32_t getMaxChannels() const 
  {
    if (m_pDevice == nullptr) {
      throw AudioException("IqAudioInput not initialised");
    }

    return m_pDevice->getMaxChannels();
  }

  [[nodiscard]] uint32_t getNumChannels() const 
  {
    if (m_pDevice == nullptr) {
      throw AudioException("IqAudioInput not initialised");
    }

    return m_pDevice->getNumChannels();
  }

  [[nodiscard]] uint32_t getSampleRate() const override
  {
    if (m_pDevice == nullptr) {
      throw AudioException("IqAudioInput not initialised");
    }

    return m_pDevice->getSampleRate();
  }

  void start(uint32_t maxPacketFrames) const override
  {
    if (m_pDevice == nullptr) {
      throw AudioException("IqAudioInput not initialised");
    }
    m_pDevice->start(maxPacketFrames);
  }
  void stop() const override
  {
    if (m_pDevice != nullptr) {
      m_pDevice->stop();
    }
  }
protected:
  AudioInputDevice* m_pDevice;
  AudioSink* m_pSink;
};
