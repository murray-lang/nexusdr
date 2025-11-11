//
// Created by murray on 12/01/25.
//

#ifndef CUTESDR_VK6HL_AUDIOSINK_H
#define CUTESDR_VK6HL_AUDIOSINK_H

#include "../../SampleTypes.h"
#include "dsp/utils/PingPongBuffers.h"

template<typename T>
class AudioSink
{
public:
  virtual ~AudioSink() = default;
  //  virtual void sink(sdrreal i, sdrreal q) = 0;
  virtual void sink(PingPongBuffers<T>& buffers, uint32_t length) = 0;
};

#endif //CUTESDR_VK6HL_AUDIOSINK_H
