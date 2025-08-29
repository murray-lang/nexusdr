//
// Created by murray on 20/8/25.
//

#ifndef CUTESDR_VK6HL_PTTSOURCE_H
#define CUTESDR_VK6HL_PTTSOURCE_H
#include "../sink/PttSink.h"

class PttSource
{
public:
  virtual ~PttSource() = default;
  virtual void connect(PttSink* pSink) = 0;
};

#endif //CUTESDR_VK6HL_PTTSOURCE_H