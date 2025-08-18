//
// Created by murray on 5/08/25.
//

#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include "AudioIo.h"
#include "./device/AudioOutputDevice.h"
#include "./device/AudioDeviceFactory.h"

class AudioOutput : public AudioIo
{
  public:
  AudioOutput() : AudioIo(), m_pDevice(nullptr)
  {
  }

  ~AudioOutput() override
  {
    delete m_pDevice;
  }

  void initialise(const AudioConfig& config) override
  {
    delete m_pDevice;
    m_pDevice = AudioDeviceFactory::createOutputDevice(config);
  }

  void start() const override
  {
    if (m_pDevice == nullptr)
    {
      throw AudioException("AudioOutput not initialised");
    }
    m_pDevice->start();
    // m_pSink->setVolume(1.0); How to do this with RtAudio???
  }

  void stop() const override
  {
    if (m_pDevice != nullptr) {
      m_pDevice->stop();
    }
  }

  [[nodiscard]] uint32_t getSampleRate() const override
  {
    if (m_pDevice == nullptr) {
      throw AudioException("IqAudioOutput not initialised");
    }

    return m_pDevice->getSampleRate();
  }

  [[nodiscard]] uint32_t addAudioData(const vsdrreal& data, uint32_t length) const
  {
    if (m_pDevice == nullptr)
    {
      return 0;
      // throw AudioException("AudioOutput not initialised");
    }
    return m_pDevice->addAudioData(data, length);
  }
protected:
  AudioOutputDevice* m_pDevice;

};

#endif //AUDIOOUTPUT_H
