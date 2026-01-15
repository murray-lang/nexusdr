//
// Created by murray on 15/1/26.
//

#pragma once
#include "SettingsBase.h"
template<typename ValueType, uint32_t ParentFeatureBit, typename ParentSettingsClass, typename StepType = ValueType>
class SteppableSetting: public SettingsBase
{
public:
  enum Features
  {
    NONE = 0,
    VALUE = 0x01,
    FINE_STEP = 0x02,
    COARSE_STEP = 0x04,
    COARSE = 0x08,
    FINE = 0x10,
    ALL = static_cast<uint32_t>(~0U)
  };
  SteppableSetting(ParentSettingsClass* parent, const char* name) :
    SettingsBase(),
    m_parent(parent),
    m_value(this, "value", 0),
    m_fineStep(this, "fine-step", 0),
    m_coarseStep(this, "coarse-step", 0)
    // m_isCoarse(this, "coarse", false),
    // m_isFine(this, "fine", true)
  {
    SettingRegistry<ParentSettingsClass>::mapping[name] = ParentFeatureBit;
    m_value.setStepAddresses(m_fineStep.getValueAddress(), m_coarseStep.getValueAddress());
  }
  SteppableSetting(ParentSettingsClass* parent, const SteppableSetting& rhs) :
    SettingsBase(rhs),
    m_parent(parent),
    m_value(this, rhs.m_value),
    m_fineStep(this, rhs.m_fineStep),
    m_coarseStep(this, rhs.m_coarseStep)
  {
    m_value.setStepAddresses(m_fineStep.getValueAddress(), m_coarseStep.getValueAddress());
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
    | m_fineStep.merge(rhs.m_fineStep, onlyChanged)
    | m_coarseStep.merge(rhs.m_coarseStep, onlyChanged);

    if (changed) {
      m_parent->setChanged(ParentFeatureBit);
    }
    return changed;
  }

  ValueType getValue() const { return m_value(); }
  StepType getFineStep() const { return m_fineStep(); }
  StepType getCoarseStep() const { return m_coarseStep(); }

  void setValue(ValueType newValue) { m_value(newValue); }
  void setFineStep(StepType newValue) { m_fineStep(newValue); }
  void setCoarseStep(StepType newValue) { m_coarseStep(newValue); }

  bool applyUpdate(SettingUpdate& update) override
  {
    if (update.isExhausted()) {
      throw SettingsException("Invalid setting path");
    }
    uint32_t feature = update.getCurrentFeature();
    const auto& val = update.getValue();
    bool settingChange = false;
    switch (feature) {
    case FINE_STEP:
      settingChange = m_fineStep.apply(val);
      break;
    case COARSE_STEP:
      settingChange = m_coarseStep.apply(val);
      break;
    case VALUE: {
      if (update.isFinal()) {
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
  Setting<ValueType, VALUE, SteppableSetting, StepType> m_value;
  Setting<StepType, FINE_STEP, SteppableSetting> m_fineStep;
  Setting<StepType, COARSE_STEP, SteppableSetting> m_coarseStep;
  // COARSE and FINE aren't intended to be settings per se.
  // They have been made settings just to simplify feature name management
  // Conveniently ignore the fact that they could be contradictory.
  // Setting<bool, COARSE, SteppableSetting> m_isCoarse;
  // Setting<bool, FINE, SteppableSetting> m_isFine;
};