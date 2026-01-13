//
// Created by murray on 9/1/26.
//

#pragma once
#include "SettingUpdate.h"
#include "SettingValue.h"
#include "SettingRegistry.h"
#include <cstdint>

class SettingsBase;

template<typename T, uint32_t FeatureBit, typename DerivedSettingsClass, typename TStep = T>
class Setting {
  T m_value;
  SettingsBase* m_parent;
  const char* m_name;
  const TStep* m_pStepValue;

public:
  Setting(DerivedSettingsClass* parent, const char* name, T initial) :
    m_value(initial),
    m_parent(parent),
    m_name(name),
    m_pStepValue(nullptr)
  {
    SettingRegistry<DerivedSettingsClass>::mapping[name] = FeatureBit;
  }

  [[nodiscard]] const char* getName() const { return m_name; }
  void setStepValueAddress(const TStep* pStep) { m_pStepValue = pStep; }
  const T* getValueAddress() const { return &m_value; }
  const T& operator()() const { return m_value; }


  bool apply(const SettingUpdate& update) {
    if (update.getMeaning() == SettingUpdate::VALUE) {
      return apply(update.getValue());
    }
    if (update.getMeaning() == SettingUpdate::DELTA) {
      return applyDelta(update.getValue());
    }
    return false;
  }

  // These are implemented in SettingsBase.h as part of avoiding a circular dependency
  void operator()(const T& newValue);
  bool apply(const SettingValue& variantValue);
  bool merge(const Setting& rhs);
private:
  bool applyDelta(const SettingValue& deltaVariant);
};
