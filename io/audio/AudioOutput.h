//
// Created by murray on 5/08/25.
//

#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H
#include <QAudioSink>

#include "AudioIo.h"
#include "device/AudioOutputDevice.h"

class AudioOutput : public AudioIo
{
  public:
  AudioOutput() : AudioIo(), m_pDevice(nullptr), m_pSink(nullptr)
  {
  }

  ~AudioOutput() override
  {
    delete m_pDevice;
    delete m_pSink;
  }

  void initialise(const AudioConfig& config)
  {
    configure(config);
    m_pDevice = new AudioOutputDevice(m_format);
    m_pSink = new QAudioSink(m_deviceInfo, m_format);
    // m_pSource->setBufferSize(m_format.sampleRate() / 5);
    // m_pSource->start();
  }

  void start() const override
  {
    if (m_pDevice == nullptr || m_pSink == nullptr)
    {
      throw AudioException("AudioOutput not initialised");
    }
    m_pDevice->start();
    m_pSink->stop();
    m_pSink->start(m_pDevice);
    m_pSink->setVolume(1.0);
  }

  void stop() const override
  {
    if (m_pSink != nullptr)
    {
      m_pSink->stop();
    }
    if (m_pDevice != nullptr)
    {
      m_pDevice->stop();
    }
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
  QAudioDevice findDevice(const std::string& searchExpression) override
  {
    return AudioDevice::findOutputDevice(searchExpression);
  }
  QAudioDevice getDefaultDevice() override { return QMediaDevices::defaultAudioOutput(); }

protected:
  AudioOutputDevice* m_pDevice;
  QAudioSink * m_pSink;

};

#endif //AUDIOOUTPUT_H
