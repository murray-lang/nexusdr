//
// Created by murray on 2025-08-24.
//

#ifndef CUTESDR_VK6HL_SETTINGDELTA_H
#define CUTESDR_VK6HL_SETTINGDELTA_H
#include "SettingPath.h"


class SingleSetting
{
public:
  enum Meaning
  {
    NONE = 0,
    VALUE,
    DELTA
  };
  SingleSetting() : m_value(0), m_meaning(NONE) {}
  SingleSetting(const SettingPath& settingPath, int32_t value, Meaning meaning) :
    m_settingPath(settingPath),
    m_value(value),
    m_meaning(meaning)
  {}
  ~SingleSetting() = default;

  [[nodiscard]] const SettingPath& getPath() const { return m_settingPath; }
  [[nodiscard]] int32_t getValue() const { return m_value; }
  [[nodiscard]] Meaning getMeaning() const { return m_meaning; }

protected:
  SettingPath m_settingPath;
  int32_t m_value;
  Meaning m_meaning;
};

#endif //CUTESDR_VK6HL_SETTINGDELTA_H