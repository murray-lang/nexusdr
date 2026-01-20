//
// Created by murray on 2025-08-24.
//

#pragma once

#include <map>

#include "SettingUpdate.h"
#include "SettingValue.h"
#include "SettingRegistry.h"
#include "Setting.h"
#include "SettingsException.h"

class SettingsBase
{
public:
  SettingsBase() : m_changed(0) {}
  SettingsBase(const SettingsBase& rhs) = default;
  virtual ~SettingsBase() = default;
  SettingsBase& operator=(const SettingsBase& rhs)
  {
    if (this != &rhs) {
      m_changed = rhs.m_changed;
    }
    return *this;
  }

  [[nodiscard]] bool isChanged() const { return !!m_changed; }
  void setChanged(uint32_t feature) { m_changed |= feature; }
  [[nodiscard]] bool hasSettingChanged(uint32_t feature) const { return (m_changed & feature) != 0; }
  virtual void clearChanged() { m_changed = 0; }
  virtual void setAllChanged() { m_changed = ~0U; }

  virtual bool applyUpdate(SettingUpdate& setting) = 0;

  template<uint32_t FeatureValue>
  static bool addFeatureToPath(const std::vector<std::string>&, std::vector<uint32_t>& featuresOut, size_t) {
    featuresOut.push_back(FeatureValue);
    return true;
  }

protected:
  template<typename T>
  bool update(T& member, const SettingValue& variantValue, uint32_t featureBit)
  {
    if (const T* pVal = std::get_if<T>(&variantValue)) {
      if (member != *pVal) {
        member = *pVal;
        m_changed |= featureBit;
        return true;
      }
    }
    return false;
  }

  template<typename DerivedSettingsClass>
  static bool resolvePathForRegisteredSetting(const std::vector<std::string>& path,
                          std::vector<uint32_t>& out,
                          size_t idx)
  {
    if (idx >= path.size()) return false;
    auto it = SettingRegistry<DerivedSettingsClass>::mapping.find(path[idx]);
    if (it != SettingRegistry<DerivedSettingsClass>::mapping.end()) {
      out.push_back(it->second);
      return true;
    }
    return false;
  }

  uint32_t m_changed;
  template<typename T, uint32_t FeatureBit, typename DerivedSettingsClass, typename TStep> friend class Setting;
};

//-------------------------------------------------------------------------------------------------
// Implementations of Setting are here rather than in Setting.h to avoid a circular dependency
//-------------------------------------------------------------------------------------------------

template<typename T, uint32_t FeatureBit, typename DerivedSettingsClass, typename TStep>
void
Setting<T, FeatureBit, DerivedSettingsClass, TStep>::operator()(const T& newValue) {
  if (m_value != newValue) {
    m_value = newValue;
    m_parent->m_changed |= FeatureBit; // Safe because of friend
  }
}

template<typename T, uint32_t FeatureBit, typename DerivedSettingsClass, typename TStep>
bool
Setting<T, FeatureBit, DerivedSettingsClass, TStep>::apply(const SettingValue& variantValue) {
  if (const T* pVal = std::get_if<T>(&variantValue)) {
      if (m_value != *pVal) {
        m_value = *pVal;
        m_parent->m_changed |= FeatureBit;
        return true;
      }
    }
    return false;
}

template<typename T, uint32_t FeatureBit, typename DerivedSettingsClass, typename TStep>
bool
Setting<T, FeatureBit, DerivedSettingsClass, TStep>::merge(const Setting& rhs, bool onlyChanged) {
  if (!onlyChanged || rhs.m_parent->hasSettingChanged(FeatureBit)) {
    if (m_value != rhs.m_value) {
      m_value = rhs.m_value;
      m_parent->m_changed |= FeatureBit;
      return true;
    }
  }
  return false;
}

template<typename T, uint32_t FeatureBit, typename DerivedSettingsClass, typename TStep>
bool
Setting<T, FeatureBit, DerivedSettingsClass, TStep>::applyDelta(const SettingValue& deltaVariant, bool isDeltaCoarse) {
  // Using visitor pattern to handle different types of delta values
  auto visitor = [&]<typename T0>(T0&& deltaFactor) -> bool {
    using V = std::decay_t<decltype(deltaFactor)>;
    if constexpr (std::is_arithmetic_v<V>) {
      if (deltaFactor != 0) {
        // Use the linked step (of type TStep) or default to 1
        TStep step = 1;
        if (isDeltaCoarse) {
          if (m_pCoarseStepValue != nullptr) {
            step = *m_pCoarseStepValue;
          }
        } else {
          if (m_pFineStepValue != nullptr) {
            step = *m_pFineStepValue;
          }
        }

        m_value += static_cast<T>(static_cast<TStep>(deltaFactor) * step);

        m_parent->m_changed |= FeatureBit;
        return true;
      }
    }
    return false;
  };

  return std::visit(visitor, deltaVariant);
}
