//
// Created by murray on 15/1/26.
//

#pragma once
#include "SettingsBase.h"
template<typename ValueType, uint32_t ParentFeatureBit, typename ParentSettingsClass, typename DeltaType = ValueType>
class SteppableSetting: public SettingsBase
{
public:
  enum Features
  {
    NONE = 0,
    VALUE = 0x01,
    FINE_DELTA = 0x02,
    COARSE_DELTA = 0x04,
    COARSE = 0x08,
    FINE = 0x10,
    ALL = static_cast<uint32_t>(~0U)
  };
  // SteppableSetting(ParentSettingsClass* parent, const char* name) :
  //   SettingsBase(),
  //   m_parent(parent),
  //   m_value(this, "value", 0),
  //   m_fineDelta(this, "fine-delta", 0),
  //   m_coarseDelta(this, "coarse-delta", 0)
  // {
  //   SettingRegistry<ParentSettingsClass>::mapping[name] = ParentFeatureBit;
  //   m_value.setStepAddresses(m_fineDelta.getValueAddress(), m_coarseDelta.getValueAddress());
  // }
  SteppableSetting(
    ParentSettingsClass* parent,
    const char* name,
    ValueType defaultValue,
    DeltaType defaultFineDelta,
    DeltaType defaultCoarseDelta
  ) :
    SettingsBase()
    , m_parent(parent)
    , m_value(this, "value", defaultValue)
    , m_fineDelta(this, "fine-delta", defaultFineDelta)
    , m_coarseDelta(this, "coarse-delta", defaultCoarseDelta)
  {
    SettingRegistry<ParentSettingsClass>::mapping[name] = ParentFeatureBit;
    m_value.setStepAddresses(m_fineDelta.getValueAddress(), m_coarseDelta.getValueAddress());
  }
  SteppableSetting(ParentSettingsClass* parent, const SteppableSetting& rhs) :
    SettingsBase(rhs),
    m_parent(parent),
    m_value(this, rhs.m_value),
    m_fineDelta(this, rhs.m_fineDelta),
    m_coarseDelta(this, rhs.m_coarseDelta)
  {
    m_value.setStepAddresses(m_fineDelta.getValueAddress(), m_coarseDelta.getValueAddress());
  }
  ~SteppableSetting() override = default;

  SteppableSetting& operator=(const SteppableSetting& rhs)
  {
    if (this != &rhs) {
      merge(rhs);
    }
    return *this;
  }

  bool merge(const SteppableSetting& rhs, bool onlyChanged = false)
  {
    bool changed = m_value.merge(rhs.m_value, onlyChanged)
    | m_fineDelta.merge(rhs.m_fineDelta, onlyChanged)
    | m_coarseDelta.merge(rhs.m_coarseDelta, onlyChanged);

    if (changed) {
      m_parent->setChanged(ParentFeatureBit);
    }
    return changed;
  }

  ValueType getValue() const { return m_value(); }
  DeltaType getFineDelta() const { return m_fineDelta(); }
  DeltaType getCoarseDelta() const { return m_coarseDelta(); }

  void setValue(ValueType newValue) { m_value(newValue); }
  void setFineDelta(DeltaType newValue) { m_fineDelta(newValue); }
  void setCoarseDelta(DeltaType newValue) { m_coarseDelta(newValue); }

  bool applyUpdate(SettingUpdate& update) override
  {
    if (update.isExhausted()) {
      throw SettingsException("Invalid setting path");
    }
    uint32_t feature = update.getCurrentFeature();
    const auto& val = update.getValue();
    bool settingChange = false;
    switch (feature) {
    case FINE_DELTA:
      settingChange = m_fineDelta.apply(val);
      break;
    case COARSE_DELTA:
      settingChange = m_coarseDelta.apply(val);
      break;
    case VALUE: {
      if (update.isAtLeaf() || update.isValue()) {
        settingChange = m_value.apply(update);
      } else {
        update.stepNextFeature();
        feature = update.getCurrentFeature();
        if (feature == COARSE) {
          settingChange = m_value.apply(update, true);
        } else if (feature == FINE) {
          settingChange = m_value.apply(update, false);
        } else {
          throw SettingsException("Invalid setting path");
        }
      }
      break;
    }
    default:
      return false;
    }
    if (settingChange) {
      m_parent->setChanged(ParentFeatureBit);
    }
    return settingChange;
  }

  static bool getFeaturePath(
   const std::vector<std::string>& featureStrings,
   std::vector<uint32_t>& out,
   size_t startIndex
   )
  {
    if (!resolvePathForRegisteredSetting<SteppableSetting>(featureStrings, out, startIndex)) {
      return false;
    }
    if (startIndex + 1 < featureStrings.size()) {
      const std::string& key = featureStrings[startIndex + 1];
      if (key == "fine") {
        out.push_back(FINE);
        return true;
      }
      if (key == "coarse") {
        out.push_back(COARSE);
        return true;
      }
      return false;
    }
    return true;
  }


protected:
  SettingsBase* m_parent;
  Setting<ValueType, VALUE, SteppableSetting, DeltaType> m_value;
  Setting<DeltaType, FINE_DELTA, SteppableSetting> m_fineDelta;
  Setting<DeltaType, COARSE_DELTA, SteppableSetting> m_coarseDelta;
  // COARSE and FINE aren't intended to be settings per se.
  // They have been made settings just to simplify feature name management
  // Conveniently ignore the fact that they could be contradictory.
  // Setting<bool, COARSE, SteppableSetting> m_isCoarse;
  // Setting<bool, FINE, SteppableSetting> m_isFine;
};