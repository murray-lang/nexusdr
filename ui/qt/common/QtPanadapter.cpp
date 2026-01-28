//
// Created by murray on 14/1/26.
//

#include "QtPanadapter.h"

#include <QValueAxis>
#include "dsp/utils/constants.h"
#include "dsp/utils/window.h"
#include <volk/volk.h>

#include "dsp/utils/pocketfft/pocketfft_hdronly.h"

QtPanadapter::QtPanadapter(QWidget* parent, const char* viewName, const char* themeName) :
  QtChartBase(parent, viewName, themeName)
  ,m_verticalCursorLine(new QGraphicsLineItem() )
  ,m_filterPassbandRect(nullptr)
{
}

void
QtPanadapter::initialise()
{
  QtChartBase::initialise();

  m_areaSeries.setUpperSeries(&m_lineSeries);

  QString seriesLineColorStr = m_pTheme->property("seriesLineColor").toString();
  auto seriesLineColor = QColor(seriesLineColorStr);
  QPen pen(seriesLineColor); //0x0080ff
  pen.setWidth(0);
  // m_spectrumLineSeries.setPen(pen);
  m_areaSeries.setPen(pen);

  QString seriesAreaColorStr = m_pTheme->property("seriesAreaColor").toString();
  auto seriesAreaColor = QColor(seriesAreaColorStr);
  QBrush seriesBrush(seriesAreaColor);
  m_areaSeries.setBrush(seriesBrush);

  m_pChart->addSeries(&m_areaSeries);

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

  QList<QAbstractAxis*> xAxes = m_pChart->axes(Qt::Horizontal);
  if (!xAxes.isEmpty()) {
    auto *xAxis = qobject_cast<QValueAxis*>(xAxes.first());
    xAxis->setRange(m_xMin, m_xMax);
    xAxis->setLabelFormat(QString("%i"));
  }
  m_pChart->axes(Qt::Vertical).first()->setRange(-110, -50);

  m_pChart->legend()->hide();

  QString cursorLineColorStr = m_pTheme->property("cursorLineColor").toString();
  m_verticalCursorLine->setPen(QPen(QColor(cursorLineColorStr), 1.5, Qt::SolidLine));
  m_pChart->scene()->addItem(m_verticalCursorLine);

}

void
QtPanadapter::updateCursorPosition(uint32_t frequency, uint32_t loCut, uint32_t hiCut)
{
  auto *axisY = qobject_cast<QValueAxis*>(m_pChart->axes(Qt::Vertical).first());
  double yMin = axisY->min();
  double yMax = axisY->max();

  QPointF p1 = m_pChart->mapToPosition(QPointF(frequency, yMin));
  QPointF p2 = m_pChart->mapToPosition(QPointF(frequency, yMax));

  m_verticalCursorLine->setLine(QLineF(p1, p2));
  m_verticalCursorLine->show();

  int32_t loCutWrtFreq = frequency + loCut;
  int32_t hioCutWrtFreq = frequency + hiCut;
  updatePassbandOverlay(loCutWrtFreq, hioCutWrtFreq);
}

void
QtPanadapter::addPassbandOverlay(int32_t loCut, int32_t hiCut)
{
  QString cursorAreaColorStr = m_pTheme->property("cursorAreaColor").toString();

  m_filterPassbandRect = new QGraphicsRectItem(loCut, 0, hiCut - loCut, 100);
  m_filterPassbandRect->setBrush(QColor(cursorAreaColorStr));
  // m_filterPassbandRect->setBrush(QColor(100, 50, 200, 80)); // Semi-transparent
  m_filterPassbandRect->setPen(Qt::NoPen);
  m_pChart->scene()->addItem(m_filterPassbandRect);
}

void
QtPanadapter::updatePassbandOverlay(int32_t loCut, int32_t hiCut)
{
  if (m_filterPassbandRect == nullptr) {
    addPassbandOverlay(loCut, hiCut);
  }
  QRectF plotArea = m_pChart->plotArea();
  double plotLoX = m_pChart->mapToPosition(QPointF(loCut, 0)).x();
  double plotHiX = m_pChart->mapToPosition(QPointF(hiCut, 0)).x();
  double width = plotHiX - plotLoX;
  QRectF r(plotLoX, plotArea.top(), width, plotArea.height());
  m_filterPassbandRect->setRect(r);
}

void
QtPanadapter::plot(const vsdrcomplex* timeSeriesData,
    uint32_t length,
    uint32_t sampleRate,
    uint32_t centreFrequency,
    bool shuffle)
{
  vsdrreal spectrum(length);
  powerSpectrum(*timeSeriesData, length, spectrum);
  plot(&spectrum, sampleRate, centreFrequency, shuffle);
}

void
QtPanadapter::plot(
    const vsdrreal* spectrumData,
    uint32_t sampleRate,
    uint32_t centreFrequency,
    bool shuffle
  )
{
  qreal plotX = centreFrequency - (sampleRate / 2);
  qreal binWidth = static_cast<qreal>(sampleRate) / static_cast<qreal>(spectrumData->size());

  QList<QPointF> spectrumPoints;
  size_t fftSize = spectrumData->size();
  if (shuffle)
  {
    for (size_t bin = fftSize/2; bin < fftSize; bin++) {
      spectrumPoints.append(QPointF(plotX, spectrumData->at(bin)));
      plotX += binWidth;
    }
    for (size_t bin = 0; bin < fftSize/2 -1; bin++) {
      spectrumPoints.append(QPointF(plotX, spectrumData->at(bin)));
      plotX += binWidth;
    }
  } else
  {
    for (size_t bin = 0; bin < fftSize; bin++) {
      spectrumPoints.append(QPointF(plotX, spectrumData->at(bin)));
      plotX += binWidth;
    }
  }
  m_lineSeries.replace(spectrumPoints);
}

void
QtPanadapter::powerSpectrum(const vsdrcomplex& timeSeries, uint32_t timeSeriesLength, vsdrreal& spectrumOut)
{
  vsdrcomplex windowed(timeSeriesLength);
  for (uint32_t i = 0; i < timeSeriesLength; i++)
  {
    windowed.at(i) = timeSeries.at(i) * static_cast<sdrreal>(hanning(i, timeSeriesLength));
  }
  pocketfft::shape_t pocketfft_shape{timeSeriesLength};
  std::vector<sdrcomplex> fftOut(timeSeriesLength);

  spectrumOut.resize(timeSeriesLength);

  pocketfft::stride_t pocketfft_stride{sizeof(sdrcomplex)};
  pocketfft::shape_t pocketfft_axes{0};

  pocketfft::c2c(
      pocketfft_shape,
      pocketfft_stride,
      pocketfft_stride,
      pocketfft_axes,
      pocketfft::FORWARD,
      windowed.data(),
      fftOut.data(),
      static_cast<sdrreal>(1.0)
  );

  volk_32fc_s32f_x2_power_spectral_density_32f(
    spectrumOut.data(),
    fftOut.data(),
    static_cast<float>(timeSeriesLength), 1.0,
    timeSeriesLength
  );
}