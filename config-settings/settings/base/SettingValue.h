#pragma once

#include <variant>
#include <string>
#include <cstdint>
#include "../Mode.h"
#include "config-settings/settings/bands/SplitBandId.h"
#include "config-settings/settings/pipeline/PipelineId.h"
#include "config-settings/settings/Agc.h"

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
  PipelineId,
  Agc
>;