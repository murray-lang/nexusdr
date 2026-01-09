//
// Created by murray on 2025-08-24.
//

#pragma once

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
  virtual void setAllChanged() { changed = ~0U; }
  [[nodiscard]] bool isChanged() const { return !!changed; }

  template<uint32_t FeatureValue>
  static bool addFeature(const std::vector<std::string>&, std::vector<uint32_t>& featuresOut, size_t) {
    featuresOut.push_back(FeatureValue);
    return true;
  }

  // virtual uint32_t getFeatureFromString(const char * strFeature) = 0;
  // virtual FeaturePath getFeaturePathFromString(const std::vector<std::string>& featureStrings) = 0;

  uint32_t changed;
};
