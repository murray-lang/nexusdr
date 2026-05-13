#pragma once
#include <CrossPlatformTypes.h>

struct IqReceiverMetering
{
  float rssiDbFs;
  uint32_t sampleRate;
  optional<float> agcGainDb;
};