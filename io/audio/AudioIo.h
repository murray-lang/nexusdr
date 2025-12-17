//
// Created by murray on 5/08/25.
//

#pragma once

#include "drivers/RtAudio/RtAudioDriver.h"
#include "config/AudioConfig.h"

class AudioIo
{
public:

  AudioIo() = default;

  virtual ~AudioIo() = default;

  virtual void configure(const AudioConfig* pConfig) = 0;

  virtual void start(uint32_t maxPacketFrames) const = 0;
  virtual void stop() const = 0;

  [[nodiscard]] virtual uint32_t getSampleRate() const = 0;

protected:
  AudioConfig m_config;
  // AudioDevice* m_pDevice;
  // AudioDevice::Format m_format;
};
