//
// Created by murray on 14/1/26.
//

#pragma once
#include "QtChartBase.h"


class QtTimeSeriesChart : public QtChartBase
{
public:
  QtTimeSeriesChart(QWidget* parent, const char* viewName, const char* themeName);
  ~QtTimeSeriesChart() override = default;

  void initialise() override;

protected:

};
