#pragma once

#include <variant>
#include <string>
#include <cstdint>
#include "../Mode.h"

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
  bool
>;