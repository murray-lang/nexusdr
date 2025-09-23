//
// Created by murray on 5/08/25.
//

#ifndef AUDIOIO_H
#define AUDIOIO_H
#include "device/AudioDevice.h"
#include "config/AudioConfig.h"

class AudioIo
{
public:

  AudioIo()// :
    // m_pDevice(nullptr)//,
    // m_format{ .sampleFormat = 0, .sampleRate = 0, .channelCount = 0 }
  {

  }

  virtual ~AudioIo() = default;

  virtual void initialise(const AudioConfig* pConfig) = 0;

  virtual void start() const = 0;
  virtual void stop() const = 0;

  [[nodiscard]] virtual uint32_t getSampleRate() const = 0;

protected:
  AudioConfig m_config;
  // AudioDevice* m_pDevice;
  // AudioDevice::Format m_format;
};

#endif //AUDIOIO_H
