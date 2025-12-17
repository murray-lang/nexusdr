//
// Created by murray on 17/12/25.
//

#pragma once
#include "AudioDriver.h"
#include "io/audio/AudioSink.h"

class AudioInputDriver : public AudioDriver
{
public:
  AudioInputDriver(const Format& format, AudioSink* pSink) : AudioDriver(format) {}

  [[nodiscard]] virtual uint32_t getMaxChannels() const = 0;
  [[nodiscard]] virtual uint32_t getNumChannels() const = 0;
};
