//
// Created by murray on 5/08/25.
//

#ifndef IQAUDIOINPUT_H
#define IQAUDIOINPUT_H
#include "device/IqAudioInputDevice.h"
#include "AudioIo.h"
#include "../../radio/config/AudioConfig.h"

class IqAudioInput : public AudioIo
{
public:
  IqAudioInput() : AudioIo(), m_pDevice(nullptr), m_pSink(nullptr)
  {
  }

  ~IqAudioInput() override
  {
    delete m_pDevice;
  }

  void initialise(const AudioConfig& config, IqSink* pSink)
  {
    m_pSink = pSink;
    configure(config);
    m_pDevice = new IqAudioInputDevice(m_deviceInfo, m_format, pSink);
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
  RtAudio::DeviceInfo findDevice(const std::string& searchExpression) override
  {
    return AudioDevice::findInputDevice(searchExpression);
  }
  RtAudio::DeviceInfo getDefaultDevice() override { return AudioDevice::findDefaultInputDevice(); }

protected:
  IqAudioInputDevice* m_pDevice;
  IqSink * m_pSink;
};

#endif //IQAUDIOINPUT_H
