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
  IqAudioInput() : AudioIo(), m_pDevice(nullptr), m_pSource(nullptr), m_pSink(nullptr)
  {
  }

  ~IqAudioInput() override
  {
    delete m_pDevice;
    delete m_pSource;
  }

  void initialise(const AudioConfig& config, IqSink* pSink)
  {
    m_pSink = pSink;
    configure(config);
    m_pDevice = new IqAudioInputDevice(m_format, pSink);
    m_pSource = new QAudioSource(m_deviceInfo, m_format);
    // m_pSource->setBufferSize(m_format.sampleRate() / 5);
    // m_pSource->start();
  }

  void start() const override
  {
    if (m_pDevice == nullptr || m_pSource == nullptr)
    {
      throw AudioException("IqAudioInput not initialised");
    }
    m_pDevice->start();
    m_pSource->stop();
    m_pSource->start(m_pDevice);
  }
  void stop() const override
  {
    if (m_pSource != nullptr)
    {
      m_pSource->stop();
    }
    if (m_pDevice != nullptr)
    {
      m_pDevice->stop();
    }
  }

protected:
  QAudioDevice findDevice(const std::string& searchExpression) override
  {
    return AudioDevice::findInputDevice(searchExpression);
  }
  QAudioDevice getDefaultDevice() override { return QMediaDevices::defaultAudioInput(); }

protected:
  IqAudioInputDevice* m_pDevice;
  QAudioSource * m_pSource;
  IqSink * m_pSink;
};

#endif //IQAUDIOINPUT_H
