//
// Created by murray on 9/1/26.
//

#pragma once
#include <map>
#include <string>
#include <cstdint>

// This will be used by classes derived from SettingsBase.
// Since the template parameter is the type of that class, only one instance of SettingRegistry
// will be instantiated for each distinct settings class.
// Also, since the `mapping` member is static, there will be only one instance of it shared
// by all instances of the derived settings class.
// This has been done to avoid needless copying of identical data.

template<typename DerivedSettingsClass>
struct SettingRegistry {
  static inline std::map<std::string, uint32_t> mapping;
};