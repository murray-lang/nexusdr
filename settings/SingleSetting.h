//
// Created by murray on 2025-08-24.
//

#ifndef CUTESDR_VK6HL_SETTINGDELTA_H
#define CUTESDR_VK6HL_SETTINGDELTA_H
#include "SettingPath.h"
#include <variant>
#include "Mode.h"

class SingleSetting
{
public:
  using ValueType = std::variant<uint32_t, int32_t, Mode::Type, float, double, std::string>;

  enum Meaning
  {
    NONE = 0,
    VALUE,
    DELTA
  };
  SingleSetting() : m_value(0), m_meaning(NONE) {}
  SingleSetting(const SettingPath& settingPath, ValueType value, const Meaning meaning) :
    m_settingPath(settingPath),
    m_value(std::move(value)),
    m_meaning(meaning)
  {}
  SingleSetting(const SingleSetting& rhs) = default;
  ~SingleSetting() = default;

  SingleSetting& operator=(const SingleSetting& rhs)
  {
    if (this != &rhs) {
      m_settingPath = rhs.m_settingPath;
      m_value = rhs.m_value;
      m_meaning = rhs.m_meaning;
    }
    return *this;
  }

  SingleSetting& setValue(ValueType value) { m_value = std::move(value); return *this; }
  [[nodiscard]] const SettingPath& getPath() const { return m_settingPath; }
  SingleSetting& setPath(const SettingPath& path) { m_settingPath = path; return *this; }
  [[nodiscard]] const ValueType& getValue() const { return m_value; }
  [[nodiscard]] Meaning getMeaning() const { return m_meaning; }

  [[nodiscard]] bool isInt() const { return std::holds_alternative<int32_t>(m_value); }
  [[nodiscard]] bool isUInt() const { return std::holds_alternative<uint32_t>(m_value); }
  [[nodiscard]] bool isDouble() const { return std::holds_alternative<double>(m_value); }
  [[nodiscard]] bool isFloat() const { return std::holds_alternative<float>(m_value); }
  [[nodiscard]] bool isString() const { return std::holds_alternative<std::string>(m_value); }
  [[nodiscard]] bool isMode() const { return std::holds_alternative<Mode::Type>(m_value); }


protected:
  SettingPath m_settingPath;
  ValueType
  m_value;
  Meaning m_meaning;
};

#endif //CUTESDR_VK6HL_SETTINGDELTA_H