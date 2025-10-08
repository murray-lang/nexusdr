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
  SettingsBase() : changed(0) {}
  SettingsBase(const SettingsBase& rhs) = default;
  virtual ~SettingsBase() = default;
  SettingsBase& operator=(const SettingsBase& rhs)
  {
    if (this != &rhs) {
      changed = rhs.changed;
    }
    return *this;
  }

  virtual bool applySetting(const SingleSetting& setting, int startIndex) = 0;

  virtual void clearChanged() { changed = 0; }
  [[nodiscard]] bool isChanged() const { return !!changed; }

  // virtual uint32_t getFeatureFromString(const char * strFeature) = 0;
  // virtual FeaturePath getFeaturePathFromString(const std::vector<std::string>& featureStrings) = 0;

  uint32_t changed;
};

#endif //CUTESDR_VK6HL_SETTINGSBASE_H