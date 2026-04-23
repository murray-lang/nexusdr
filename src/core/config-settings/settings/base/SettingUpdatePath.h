//
// Created by murray on 2025-08-24.
//

#pragma once

#include <cstdint>

#include "etl/vector.h"


#ifdef USE_ETL_COLLECTIONS
#include "etl/vector.h"

using FeatureVector = etl::vector<uint32_t, MAX_SETTING_DEPTH>;
#else
#include <vector>

using FeatureVector = std::vector<uint32_t>;
#endif

class SettingUpdatePath
{
public:
  SettingUpdatePath() = default;
  SettingUpdatePath(const SettingUpdatePath& rhs) = default;
  explicit SettingUpdatePath(const FeatureVector& features) : m_features(features) {}
  virtual ~SettingUpdatePath() = default;

  SettingUpdatePath& operator=(const SettingUpdatePath& rhs) = default;
  // bool operator==(const SettingPath& rhs) const;
  [[nodiscard]] const FeatureVector& getFeatures() const { return m_features; }
  void setFeatures(const FeatureVector& features) { m_features = features; }

  bool operator==(const SettingUpdatePath& rhs) const
  {
    return m_features == rhs.m_features;
  }

  // static FeaturePath fromString(const std::string& str)
  // {
  //   std::vector<std::string> parts = split(str);
  //
  // }

protected:
  FeatureVector m_features;
};
