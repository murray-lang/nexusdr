#pragma once

#include <cstdint>
#include <CrossPlatformTypes.h>
#include "../Mode.h"
#include "../AgcSpeed.h"
#include "../bands/SplitBandId.h"
#include "../pipeline/PipelineId.h"

/**
 * The unified variant type for all setting data in the system.
 */
using SettingValue = variant<
  uint64_t,
  int64_t,
  uint32_t,
  int32_t,
  Mode::Type,
  float,
  double,
  BandNameString,
  ModeNameString,
  bool,
  SplitBandId,
  PipelineId,
  AgcSpeed
>;