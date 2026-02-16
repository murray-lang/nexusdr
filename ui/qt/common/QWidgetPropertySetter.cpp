//
// Created by murray on 6/2/26.
//

#include "QWidgetPropertySetter.h"
#include <QStyle>


void
QWidgetPropertySetter::setWidgetProperties(QWidget* widget, std::span<const WidgetProperty> props, bool repolish)
{
  for (const auto& [k, v] : props)
    setWidgetProperty(widget, k, v, false);

  if (repolish) {
    repolishRecursively(widget);
  }
}

void
QWidgetPropertySetter::setWidgetProperty(QWidget* widget, const std::string& key, const PropertyValue& value, bool repolish)
{
  QVariant v;
  std::visit([&](const auto& x) {
    using T = std::decay_t<decltype(x)>;
    if constexpr (std::is_same_v<T, bool>)
      v = QVariant{x};
    else if constexpr (std::is_same_v<T, int64_t>)
      v = QVariant::fromValue<qlonglong>(static_cast<qlonglong>(x));
    else if constexpr (std::is_same_v<T, double>)
      v = QVariant{x};
    else if constexpr (std::is_same_v<T, std::string>)
      v = QVariant{QString::fromStdString(x)};
    else if constexpr (std::is_same_v<T, const char *>)
      v = QVariant{QString(x)};
  }, value);

  widget->setProperty(key.c_str(), v);

  if (repolish) {
    repolishRecursively(widget);
  }
}

void
QWidgetPropertySetter::repolishRecursively(QWidget* w)
{
  w->style()->unpolish(w);
  w->style()->polish(w);
  w->update();

  const auto children = w->findChildren<QWidget*>(QString{}, Qt::FindDirectChildrenOnly);
  for (QWidget* child : children)
    repolishRecursively(child);
}