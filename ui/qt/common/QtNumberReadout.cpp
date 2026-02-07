//
// Created by murray on 28/1/26.
//

#include "QtNumberReadout.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QStyle>

#include <sstream>
#include <iomanip>
#include <string>

QtNumberReadout::QtNumberReadout(QWidget* parent)
  : QWidget(parent)
  // , m_title(new QLabel(this))
  , m_value(new QLabel(this))
  , m_locale("")
{
  setAttribute(Qt::WA_StyledBackground, true);

  auto* v = new QVBoxLayout(this);
  v->setContentsMargins(0, 0, 0, 0);
  v->setSpacing(0);

  m_oss.imbue(m_locale);
  // m_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

  m_value->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  m_value->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_value->setText("0");

  // v->addWidget(m_title);
  v->addWidget(m_value);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  setProperty("role", "numberReadout");
  m_value->setProperty("role", "numberReadoutValue");
}

// void QtNumberReadout::setTitle(const QString& title)
// {
//   m_title->setText(title);
// }


void
QtNumberReadout::setValue(int64_t value)
{
  sdrreal v = static_cast<sdrreal>(value) / 1000000.0;
  setValue(v);
}

void
QtNumberReadout::setValue(sdrreal value)
{
  m_oss.str({});
  m_oss.clear();
  m_oss << std::fixed << std::setprecision(6) << value;
  setValueText(m_oss.str());
}

void
QtNumberReadout::setValueText(const std::string& value)
{
  m_value->setText(value.c_str());
}

void QtNumberReadout::setStyleProperties(std::span<const WidgetProperty> props)
{
  QWidgetPropertySetter::setWidgetProperties(this, props);
  QWidgetPropertySetter::setWidgetProperties(m_value, props);
}
