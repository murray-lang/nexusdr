//
// Created by murray on 6/2/26.
//

#pragma once

#include <QWidget>
#include <variant>
#include <string>

using PropertyValue = std::variant<bool, int64_t, double, std::string, const char*>;
using WidgetProperty = std::pair<std::string, PropertyValue>;

class QWidgetPropertySetter
{
public:
  static void setWidgetProperties(QWidget* widget, std::span<const WidgetProperty> props, bool repolish = true);
  static void setWidgetProperty(QWidget* widget, const std::string& key, const PropertyValue& value, bool repolish = true);

  static void repolishRecursively(QWidget* w);

};
