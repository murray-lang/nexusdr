//
// Created by murray on 28/1/26.
//

#pragma once

#include <QWidget>

#include "SampleTypes.h"
#include <locale>

class QLabel;

class QtNumberReadout final : public QWidget
{
  Q_OBJECT
public:
  using StyleValue = std::variant<bool, int64_t, double, std::string>;
  using StyleProperty = std::pair<std::string, StyleValue>;

  explicit QtNumberReadout(QWidget* parent = nullptr);

  // void setTitle(const QString& title);
  void setValue(int64_t value);
  void setValue(sdrreal value);
  void setValueText(const std::string& value);

  // QLabel* titleLabel() const noexcept { return m_title; }
  QLabel* valueLabel() const noexcept { return m_value; }

  void applyStyleProperties(std::span<const StyleProperty> props);

private:
  void repolishRecursively(QWidget* w);
  void setDynamicProperty(QObject* obj, const std::string& key, const StyleValue& value);

  // QLabel* m_title;
  QLabel* m_value;
  std::locale m_locale;
  std::ostringstream m_oss;
};