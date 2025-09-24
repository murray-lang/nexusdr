//
// Created by murray on 2025-08-24.
//

#ifndef CUTESDR_VK6HL_SETTINGSBASE_H
#define CUTESDR_VK6HL_SETTINGSBASE_H
#include <cstdint>

#include "SingleSetting.h"
#include "SettingsException.h"

class SettingsBase
{
public:
  virtual ~SettingsBase() = default;

  virtual void applySetting(const SingleSetting& setting) = 0;

  // virtual uint32_t getFeatureFromString(const char * strFeature) = 0;
  // virtual FeaturePath getFeaturePathFromString(const std::vector<std::string>& featureStrings) = 0;
};

#endif //CUTESDR_VK6HL_SETTINGSBASE_H