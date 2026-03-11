//
// Created by murray on 9/1/26.
//

#pragma once
#include "SettingUpdate.h"
#include "SettingValue.h"
#include "SettingRegistry.h"
#include <cstdint>

class SettingsBase;

template<typename T, uint32_t ParentFeatureBit, typename ParentSettingsClass, typename TStep = T>
class Setting {
  T m_value;
  SettingsBase* m_parent;
  const char* m_name;
  const TStep* m_pFineStepValue;
  const TStep* m_pCoarseStepValue;

public:
  Setting(ParentSettingsClass* parent, const char* name, T initial) :
    m_value(initial),
    m_parent(parent),
    m_name(name),
    m_pFineStepValue(nullptr),
    m_pCoarseStepValue(nullptr)
  {
    SettingRegistry<ParentSettingsClass>::mapping[name] = ParentFeatureBit;
  }
  Setting(ParentSettingsClass* parent, const Setting& rhs) :
    m_value(rhs.m_value),
    m_parent(parent),
    m_name(rhs.m_name),
    m_pFineStepValue(nullptr),
    m_pCoarseStepValue(nullptr)
  {
  }
  Setting(const Setting& rhs) = delete;

  Setting& operator=(const Setting& rhs)
  {
    if (this != &rhs) {
      // Don't copy pointers to parent or step values (Causes BUGS!)
     merge(rhs);
    }
    return *this;
  }

  [[nodiscard]] const char* getName() const { return m_name; }
  void setFineStepValueAddress(const TStep* pStep) { m_pFineStepValue = pStep; }
  void setCoarseStepValueAddress(const TStep* pStep) { m_pCoarseStepValue = pStep; }
  void setStepAddresses(const TStep* pFineStepValue, const TStep* pCoarseStepValue)
  {
    m_pFineStepValue = pFineStepValue;
    m_pCoarseStepValue = pCoarseStepValue;
  }
  const T* getValueAddress() const { return &m_value; }
  const T& operator()() const { return m_value; }


  bool apply(const SettingUpdate& update, bool isDeltaCoarse = false) {
    if (update.getMeaning() == SettingUpdate::VALUE) {
      return apply(update.getValue());
    }
    if (update.getMeaning() == SettingUpdate::DELTA) {
      return applyDelta(update.getValue(), isDeltaCoarse);
    }
    return false;
  }

  // These are implemented in SettingsBase.h as part of avoiding a circular dependency
  void operator()(const T& newValue);
  bool apply(const SettingValue& variantValue);
  bool merge(const Setting& rhs, bool onlyChanged = false);
private:
  bool applyDelta(const SettingValue& deltaVariant, bool isDeltaCoarse = false);
};
