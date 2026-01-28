//
// Created by murray on 14/1/26.
//

#include "QtTimeSeriesChart.h"
#include <QValueAxis>
#include <QGraphicsLayout>

QtTimeSeriesChart::QtTimeSeriesChart(QWidget* parent, const char* viewName, const char* themeName) :
  QtChartBase(parent, viewName, themeName)
{}

void
QtTimeSeriesChart::initialise()
{
  QtChartBase::initialise();

  applyTheme();
  for (auto* axis : m_pChart->axes()) {
    axis->setLabelsVisible(false);
  }
  m_pChart->setMargins(QMargins(0, 0, 0, 0));
  m_pChart->layout()->setContentsMargins(0, 0, 0, 0);
  m_pChart->setBackgroundRoundness(0);

  m_pChart->axes(Qt::Horizontal).first()->setRange(m_xMin / 100.0, m_xMax/100.0);
  m_pChart->axes(Qt::Vertical).first()->setRange(-0.05, 0.05);

  m_pChart->legend()->hide();
}