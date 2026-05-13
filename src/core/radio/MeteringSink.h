#pragma once
#include "receiver/IqReceiverMetering.h"

class MeteringSink
{
public:
  virtual ~MeteringSink() = default;
  virtual void updateRxMetering(const IqReceiverMetering& metering) = 0;
};
