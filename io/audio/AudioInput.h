//
// Created by murray on 5/08/25.
//

#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H
#include "AudioException.h"
#include "./device/AudioDeviceFactory.h"
#include "AudioIo.h"
#include "IqSink.h"
#include "config/AudioConfig.h"

template<typename T>
class AudioInput : public AudioIo
{
public:
  explicit AudioInput(AudioSink<T>* pSink) : AudioIo(), m_pDevice(nullptr), m_pSink(pSink)
  {
  }

  ~AudioInput() override
  {
    delete m_pDevice;
  }

  void initialise(const AudioConfig* pConfig) override
  {
    // delete m_pDevice;
    m_pDevice = AudioDeviceFactory::createInputDevice<T>(pConfig, m_pSink);
  }

  [[nodiscard]] uint32_t getSampleRate() const override
  {
    if (m_pDevice == nullptr) {
      throw AudioException("IqAudioInput not initialised");
    }

    return m_pDevice->getSampleRate();
  }

  void start() const override
  {
    if (m_pDevice == nullptr) {
      throw AudioException("IqAudioInput not initialised");
    }
    m_pDevice->start();
  }
  void stop() const override
  {
    if (m_pDevice != nullptr) {
      m_pDevice->stop();
    }
  }
protected:
  AudioInputDevice<T>* m_pDevice;
  AudioSink<T>* m_pSink;
};

#endif //AUDIOINPUT_H
