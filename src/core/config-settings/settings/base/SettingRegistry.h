#pragma once
#include "../SettingsCrossPlatformTypes.h"
#include <cstdint>

// This will be used by classes derived from SettingsBase.
// Since the template parameter is the type of that class, only one instance of SettingRegistry
// will be instantiated for each distinct settings class.
// Also, since the `mapping` member is static, there will be only one instance of it shared
// by all instances of the derived settings class.
// This has been done to avoid needless copying of identical data.

template<typename DerivedSettingsClass>
struct SettingRegistry {
  static inline FeatureStringMap mapping;
};