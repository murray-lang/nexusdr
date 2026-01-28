//
// Created by murray on 14/1/26.
//

#pragma once

#include <QChartView>
#include <QLineSeries>
#include <QWidget>

#include "QtChartTheme.h"
#include "SampleTypes.h"


class QtChartBase
{
  // Q_OBJECT
public:
  QtChartBase(QWidget* parent, const char* viewName, const char* themeName);
  virtual ~QtChartBase() = default;

  virtual void initialise();

  virtual void plot(const vsdrreal& data, uint32_t length);
  virtual void plot(const vsdrcomplex data, uint32_t length);

  void setSeriesXMinMax(uint32_t min, uint32_t max);

protected:
  virtual void applyTheme();


  QWidget* m_pParent;
  QString m_viewName;
  QString m_themeName;
  QChartView* m_pChartView;
  QChart* m_pChart;
  QtChartTheme* m_pTheme;
  QLineSeries m_lineSeries;
  uint32_t m_xMin;
  uint32_t m_xMax;
};
