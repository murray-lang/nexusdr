//
// Created by murray on 17/12/25.
//

#pragma once
#include "AudioDriver.h"

class AudioOutputDriver : public AudioDriver
{
public:
  AudioOutputDriver(const Format& format) : AudioDriver(format) {}
  virtual uint32_t addAudioData(const vsdrreal& data, uint32_t length, uint32_t numChannels) = 0;
};