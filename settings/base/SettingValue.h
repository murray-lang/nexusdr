#pragma once

#include <variant>
#include <string>
#include <cstdint>
#include "../Mode.h"
#include "settings/bands/SplitBandId.h"
#include "settings/pipeline/PipelineId.h"

/**
 * The unified variant type for all setting data in the system.
 */
using SettingValue = std::variant<
  uint64_t,
  int64_t,
  uint32_t,
  int32_t,
  Mode::Type,
  float,
  double,
  std::string,
  bool,
  SplitBandId,
  PipelineId
>;