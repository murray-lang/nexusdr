//
// Created by murray on 2025-08-24.
//

#pragma once

#include "SettingUpdatePath.h"
#include "SettingValue.h"
#include <variant>
#include "Mode.h"

class SettingUpdate
{
public:
  enum Meaning { NONE = 0, VALUE, DELTA };

  SettingUpdate() : m_value(0), m_meaning(NONE), m_cursor(0) {}
  SettingUpdate(const SettingUpdatePath& settingPath, SettingValue value, const Meaning meaning) :
    m_settingPath(settingPath),
    m_value(std::move(value)),
    m_meaning(meaning),
    m_cursor(0)
  {}
  SettingUpdate(const SettingUpdate& rhs) = default;
  ~SettingUpdate() = default;

  SettingUpdate& operator=(const SettingUpdate& rhs)
  {
    if (this != &rhs) {
      m_settingPath = rhs.m_settingPath;
      m_value = rhs.m_value;
      m_meaning = rhs.m_meaning;
    }
    return *this;
  }

  [[nodiscard]] uint32_t currentFeature() const {
    const auto& features = m_settingPath.getFeatures();
    return (m_cursor < features.size()) ? features[m_cursor] : 0; // 0 = NONE
  }

  void stepNext() {
    m_cursor++;
  }

  [[nodiscard]] bool isDone() const
  {
    return m_cursor >= m_settingPath.getFeatures().size();
  }


  SettingUpdate& setValue(SettingValue value) { m_value = std::move(value); return *this; }
  [[nodiscard]] const SettingUpdatePath& getPath() const { return m_settingPath; }
  SettingUpdate& setPath(const SettingUpdatePath& path) { m_settingPath = path; return *this; }
  [[nodiscard]] const SettingValue& getValue() const { return m_value; }
  [[nodiscard]] Meaning getMeaning() const { return m_meaning; }

  [[nodiscard]] bool isInt() const { return std::holds_alternative<int32_t>(m_value); }
  [[nodiscard]] bool isUInt() const { return std::holds_alternative<uint32_t>(m_value); }
  [[nodiscard]] bool isDouble() const { return std::holds_alternative<double>(m_value); }
  [[nodiscard]] bool isFloat() const { return std::holds_alternative<float>(m_value); }
  [[nodiscard]] bool isString() const { return std::holds_alternative<std::string>(m_value); }
  [[nodiscard]] bool isBool() const { return std::holds_alternative<bool>(m_value); }
  [[nodiscard]] bool isMode() const { return std::holds_alternative<Mode::Type>(m_value); }


protected:
  SettingUpdatePath m_settingPath;
  SettingValue  m_value;
  Meaning m_meaning;
  size_t m_cursor;
};
