//
// Created by murray on 5/08/25.
//

#ifndef IQAUDIOINPUT_H
#define IQAUDIOINPUT_H
#include "./device/IqAudioInputDevice.h"
#include "./device/AudioDeviceFactory.h"
#include "AudioIo.h"
#include "../../radio/config/AudioConfig.h"

class IqAudioInput : public AudioIo
{
public:
  explicit IqAudioInput(IqSink* pSink) : AudioIo(), m_pDevice(nullptr), m_pSink(pSink)
  {
  }

  ~IqAudioInput() override
  {
    delete m_pDevice;
  }

  void initialise(const AudioConfig& config) override
  {
    // delete m_pDevice;
    m_pDevice = AudioDeviceFactory::createInputDevice(config, m_pSink);
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
  IqAudioInputDevice* m_pDevice;
  IqSink* m_pSink;
};

#endif //IQAUDIOINPUT_H
