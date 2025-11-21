//
// Created by murray on 2025-08-24.
//

#pragma once

#include <cstdint>
#include <string>
#include <vector>

class SettingPath
{
public:
  SettingPath() = default;
  SettingPath(const SettingPath& rhs) = default;
  SettingPath(const std::vector<uint32_t>& features) : m_features(features) {}
  virtual ~SettingPath() = default;

  SettingPath& operator=(const SettingPath& rhs) = default;
  // bool operator==(const SettingPath& rhs) const;
  [[nodiscard]] const std::vector<uint32_t>& getFeatures() const { return m_features; }

  // static FeaturePath fromString(const std::string& str)
  // {
  //   std::vector<std::string> parts = split(str);
  //
  // }

protected:
  std::vector<uint32_t> m_features;
};
