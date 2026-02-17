//
// Created by murray on 14/1/26.
//

#pragma once
#include <QAreaSeries>

#include "QtChartBase.h"
#include "config-settings/settings/pipeline/PipelineId.h"


class QtPanadapter : public QtChartBase
{
public:
  QtPanadapter(QWidget* parent, const char* viewName, const char* themeName);
  ~QtPanadapter() override = default;

  void initialise() override;

  void showCursorB(bool show);

  void addPassbandOverlayA(int64_t loCut, int64_t hiCut);
  void addPassbandOverlayB(int64_t loCut, int64_t hiCut);
  void updatePassbandOverlayA(int64_t loCut, int64_t hiCut);
  void updatePassbandOverlayB(int64_t loCut, int64_t hiCut);

  void plot(
    const vsdrcomplex* timeSeriesData,
    uint32_t length,
    uint32_t sampleRate,
    int64_t centreFrequency,
    bool shuffle = true
  );

  void plot(
    const vsdrreal* spectrumData,
    uint32_t sampleRate,
    int64_t centreFrequency,
    bool shuffle = true
  );

  void updateCursorPositionA(int64_t frequency, int32_t loCut, int32_t hiCut);
  void updateCursorPositionB(int64_t frequency, int32_t loCut, int32_t hiCut);

protected:
  static void powerSpectrum(const std::vector<sdrcomplex>& timeSeries, uint32_t timeSeriesLength, vsdrreal& spectrumOut);

  void refreshOverlays();

  struct CursorState {
    bool valid = false;
    bool visible = false;
    int64_t frequency = 0;
    int32_t loCut = 0;
    int32_t hiCut = 0;
  };

  CursorState m_cursorA;
  CursorState m_cursorB;

  QAreaSeries m_areaSeries;
  QGraphicsLineItem* m_verticalCursorLineA;
  QGraphicsRectItem* m_filterPassbandRectA;
  QGraphicsLineItem* m_verticalCursorLineB;
  QGraphicsRectItem* m_filterPassbandRectB;
};
