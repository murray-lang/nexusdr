//
// Created by murray on 14/1/26.
//

#pragma once
#include <QAreaSeries>

#include "QtChartBase.h"


class QtPanadapter : public QtChartBase
{
public:
  QtPanadapter(QWidget* parent, const char* viewName, const char* themeName);
  ~QtPanadapter() override = default;

  void initialise() override;

  void addPassbandOverlay(int32_t loCut, int32_t hiCut);
  void updatePassbandOverlay(int32_t loCut, int32_t hiCut);

  void plot(
    const vsdrcomplex* timeSeriesData,
    uint32_t length,
    uint32_t sampleRate,
    uint32_t centreFrequency,
    bool shuffle = true
  );

  void plot(
    const vsdrreal* spectrumData,
    uint32_t sampleRate,
    uint32_t centreFrequency,
    bool shuffle = true
  );

  void updateCursorPosition(uint32_t frequency, uint32_t loCut, uint32_t hiCut);

protected:
  static void powerSpectrum(const std::vector<sdrcomplex>& timeSeries, uint32_t timeSeriesLength, vsdrreal& spectrumOut);

  QAreaSeries m_areaSeries;
  QGraphicsLineItem* m_verticalCursorLine;
  QGraphicsRectItem* m_filterPassbandRect;
};
