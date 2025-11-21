//
// Created by murray on 20/8/25.
//

#pragma once

#include "PttSink.h"

class PttSource
{
public:
  virtual ~PttSource() = default;
  virtual void connect(PttSink* pSink) = 0;
};
