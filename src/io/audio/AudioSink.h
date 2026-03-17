//
// Created by murray on 18/11/25.
//

#pragma once
#include <cstdint>

#include "core/SampleTypes.h"

class AudioSink
{
public:
  virtual ~AudioSink() = default;
  virtual uint32_t sinkAudio(const vsdrreal& samples, uint32_t length, uint32_t numChannels) = 0;
};
