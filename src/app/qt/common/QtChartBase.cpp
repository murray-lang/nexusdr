//
// Created by murray on 14/1/26.
//

#include "QtChartBase.h"

QtChartBase::QtChartBase(QWidget* parent, const char* viewName, const char* themeName)  :
  m_pParent(parent)
  ,m_viewName(viewName)
  ,m_themeName(themeName)
  ,m_pChartView(nullptr)
  ,m_pChart(nullptr)
  ,m_pTheme(nullptr)
  ,m_xMin(0)
  ,m_xMax(0)
{
}

void
QtChartBase::initialise()
{
  m_pChartView = m_pParent->findChild<QChartView*>(m_viewName);
  if (m_pChartView == nullptr) {
    throw std::runtime_error("No chart view '" + m_viewName.toStdString() + "' found");
  }
  m_pChartView->setRenderHint(QPainter::Antialiasing);

  m_pChart = m_pChartView->chart();
  if (m_pChart == nullptr) {
    throw std::runtime_error("No chart found in view '" + m_viewName.toStdString() + "'");
  }
  m_pTheme = m_pParent->findChild<QtChartTheme*>(m_themeName);
  if (m_pTheme == nullptr) {
    throw std::runtime_error("No theme '" + m_themeName.toStdString() + "' found");
  }
  m_pChart->addSeries(&m_lineSeries);
  applyTheme();
}

void
QtChartBase::applyTheme()
{
  m_pTheme->ensurePolished();

  QString backgroundColorStr = m_pTheme->property("backgroundColor").toString();
  auto backgroundColor = QColor(backgroundColorStr);
  m_pChart->setBackgroundBrush(backgroundColor);

  QString plotAreaColorStr = m_pTheme->property("plotAreaColor").toString();
  m_pChart->setPlotAreaBackgroundBrush(QBrush(QColor(plotAreaColorStr)));
  m_pChart->setPlotAreaBackgroundVisible(true);

  QString seriesLineColorStr = m_pTheme->property("seriesLineColor").toString();
  auto seriesLineColor = QColor(seriesLineColorStr);
  QPen pen(seriesLineColor); //0x0080ff
  pen.setWidth(0);
  m_lineSeries.setPen(pen);

  QString gridColorStr = m_pTheme->property("gridColor").toString();
  QPen gridPen(gridColorStr);


  QString textColorStr = m_pTheme->property("textColor").toString();
  QColor textColor(textColorStr);
  m_pChart->setTitleBrush(QBrush(textColor));

  m_pChart->createDefaultAxes();

  for (auto* axis : m_pChart->axes()) {
    axis->setGridLinePen(gridPen);
    axis->setLinePen(gridPen);
    axis->setLabelsColor(textColor);
  }
}

void
QtChartBase::plot(const RealSamplesMax& data, uint32_t length)
{
  setSeriesXMinMax(0, length);
  QList<QPointF> seriesPoints;
  uint32_t plotX = 0;
  for (uint32_t i = 0; i < length; i++) {
    seriesPoints.append(QPointF(plotX++, data.at(i)));
  }
  m_lineSeries.replace(seriesPoints);
}

void
QtChartBase::plot(const ComplexSamplesMax& data, uint32_t length)
{
  setSeriesXMinMax(0, length);
  QList<QPointF> seriesPoints;
  uint32_t plotX = 0;
  for (uint32_t i = 0; i < length; i++) {
    seriesPoints.append(QPointF(plotX++, std::abs(data.at(i))));
  }
  m_lineSeries.replace(seriesPoints);
}

void
QtChartBase::setSeriesXMinMax(int64_t min, int64_t max)
{
  if ( min != m_xMin || max != m_xMax) {
    m_xMin = min;
    m_xMax = max;

    m_pChart->axes(Qt::Horizontal)
      .first()->setRange(static_cast<qlonglong>(m_xMin), static_cast<qlonglong>(m_xMax));
  }
}