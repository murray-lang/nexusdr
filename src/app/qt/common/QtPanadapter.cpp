//
// Created by murray on 14/1/26.
//

#include "QtPanadapter.h"

#include <QValueAxis>
#include "core/dsp/utils/constants.h"
#include "core/dsp/utils/window.h"
#include <volk/volk.h>
#include <cmath>

#include "core/dsp/utils/pocketfft/pocketfft_hdronly.h"

QtPanadapter::QtPanadapter(QWidget* parent, const char* viewName, const char* themeName) :
  QtChartBase(parent, viewName, themeName)
  ,m_verticalCursorLineA(new QGraphicsLineItem() )
  ,m_filterPassbandRectA(nullptr)
  ,m_verticalCursorLineB(new QGraphicsLineItem() )
  ,m_filterPassbandRectB(nullptr)
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

  // Recalculate cursor/overlay positions whenever the chart mapping changes
  if (m_pChartView) {
    QObject::connect(m_pChart, &QChart::plotAreaChanged, m_pChartView, [this](const QRectF&) {
      refreshOverlays();
    });

    if (!m_pChart->axes(Qt::Horizontal).isEmpty()) {
      if (auto* xAxis = qobject_cast<QValueAxis*>(m_pChart->axes(Qt::Horizontal).first())) {
        QObject::connect(xAxis, &QValueAxis::rangeChanged, m_pChartView, [this](qreal, qreal) {
          refreshOverlays();
        });
      }
    }
    if (!m_pChart->axes(Qt::Vertical).isEmpty()) {
      if (auto* yAxis = qobject_cast<QValueAxis*>(m_pChart->axes(Qt::Vertical).first())) {
        QObject::connect(yAxis, &QValueAxis::rangeChanged, m_pChartView, [this](qreal, qreal) {
          refreshOverlays();
        });
      }
    }
  }

  QString cursorALineColorStr = m_pTheme->property("cursorALineColor").toString();
  m_verticalCursorLineA->setPen(QPen(QColor(cursorALineColorStr), 1.5, Qt::SolidLine));
  m_pChart->scene()->addItem(m_verticalCursorLineA);

  QString cursorBLineColorStr = m_pTheme->property("cursorBLineColor").toString();
  m_verticalCursorLineB->setPen(QPen(QColor(cursorBLineColorStr), 1.5, Qt::SolidLine));
  m_pChart->scene()->addItem(m_verticalCursorLineB);
}

void
QtPanadapter::showCursorB(bool show)
{
  if (m_verticalCursorLineB) {
    if (show) {
      m_verticalCursorLineB->show();

    } else {
      m_verticalCursorLineB->hide();
    }
  }
  if (m_filterPassbandRectB) {
    if (show) {
      m_filterPassbandRectB->show();

    } else {
      m_filterPassbandRectB->hide();
    }
  }
}

void
QtPanadapter::refreshOverlays()
{
  if (m_cursorA.valid && m_cursorA.visible) {
    updateCursorPositionA(m_cursorA.frequency, m_cursorA.loCut, m_cursorA.hiCut);
  }
  if (m_cursorB.valid && m_cursorB.visible) {
    updateCursorPositionB(m_cursorB.frequency, m_cursorB.loCut, m_cursorB.hiCut);
  }
}

void
QtPanadapter::updateCursorPositionA(int64_t frequency, int32_t loCut, int32_t hiCut)
{
  m_cursorA.valid = true;
  m_cursorA.visible = true;
  m_cursorA.frequency = frequency;
  m_cursorA.loCut = loCut;
  m_cursorA.hiCut = hiCut;

  auto *axisY = qobject_cast<QValueAxis*>(m_pChart->axes(Qt::Vertical).first());
  double yMin = axisY->min();
  double yMax = axisY->max();

  QPointF p1 = m_pChart->mapToPosition(QPointF(static_cast<qreal>(frequency), yMin));
  QPointF p2 = m_pChart->mapToPosition(QPointF(static_cast<qreal>(frequency), yMax));

  // qDebug() << "updateCursorPositionA: frequency = " << frequency << " p1 = " << p1.x() << " p2 = " << p2.x();

  m_verticalCursorLineA->setLine(QLineF(p1, p2));
  m_verticalCursorLineA->show();

  int64_t loCutWrtFreq = frequency + loCut;
  int64_t hioCutWrtFreq = frequency + hiCut;
  updatePassbandOverlayA(loCutWrtFreq, hioCutWrtFreq);
}

void
QtPanadapter::updateCursorPositionB(int64_t frequency, int32_t loCut, int32_t hiCut)
{
  m_cursorB.valid = true;
  m_cursorB.frequency = frequency;
  m_cursorB.loCut = loCut;
  m_cursorB.hiCut = hiCut;

  auto *axisY = qobject_cast<QValueAxis*>(m_pChart->axes(Qt::Vertical).first());
  double yMin = axisY->min();
  double yMax = axisY->max();

  QPointF p1 = m_pChart->mapToPosition(QPointF(static_cast<qreal>(frequency), yMin));
  QPointF p2 = m_pChart->mapToPosition(QPointF(static_cast<qreal>(frequency), yMax));

  m_verticalCursorLineB->setLine(QLineF(p1, p2));
  m_verticalCursorLineB->show();

  int64_t loCutWrtFreq = frequency + loCut;
  int64_t hioCutWrtFreq = frequency + hiCut;
  updatePassbandOverlayB(loCutWrtFreq, hioCutWrtFreq);
}

void
QtPanadapter::addPassbandOverlayA(int64_t loCut, int64_t hiCut)
{
  QString cursorAAreaColorStr = m_pTheme->property("cursorAAreaColor").toString();

  m_filterPassbandRectA = new QGraphicsRectItem(static_cast<qreal>(loCut), 0, static_cast<qreal>(hiCut - loCut) , 100);
  m_filterPassbandRectA->setBrush(QColor(cursorAAreaColorStr));
  // m_filterPassbandRect->setBrush(QColor(100, 50, 200, 80)); // Semi-transparent
  m_filterPassbandRectA->setPen(Qt::NoPen);
  m_pChart->scene()->addItem(m_filterPassbandRectA);
}

void
QtPanadapter::addPassbandOverlayB(int64_t loCut, int64_t hiCut)
{
  QString cursorBAreaColorStr = m_pTheme->property("cursorBAreaColor").toString();

  m_filterPassbandRectB = new QGraphicsRectItem(static_cast<qreal>(loCut), 0, static_cast<qreal>(hiCut - loCut), 100);
  m_filterPassbandRectB->setBrush(QColor(cursorBAreaColorStr));
  // m_filterPassbandRect->setBrush(QColor(100, 50, 200, 80)); // Semi-transparent
  m_filterPassbandRectB->setPen(Qt::NoPen);
  m_pChart->scene()->addItem(m_filterPassbandRectB);
}

void
QtPanadapter::updatePassbandOverlayA(int64_t loCut, int64_t hiCut)
{
  if (m_filterPassbandRectA == nullptr) {
    addPassbandOverlayA(loCut, hiCut);
  }
  QRectF plotArea = m_pChart->plotArea();
  double plotLoX = m_pChart->mapToPosition(QPointF(static_cast<qreal>(loCut), 0)).x();
  double plotHiX = m_pChart->mapToPosition(QPointF(static_cast<qreal>(hiCut), 0)).x();
  double width = plotHiX - plotLoX;
  QRectF r(plotLoX, plotArea.top(), width, plotArea.height());
  m_filterPassbandRectA->setRect(r);
}

void
QtPanadapter::updatePassbandOverlayB(int64_t loCut, int64_t hiCut)
{
  if (m_filterPassbandRectB == nullptr) {
    addPassbandOverlayB(loCut, hiCut);
  }
  QRectF plotArea = m_pChart->plotArea();
  double plotLoX = m_pChart->mapToPosition(QPointF(static_cast<qreal>(loCut), 0)).x();
  double plotHiX = m_pChart->mapToPosition(QPointF(static_cast<qreal>(hiCut), 0)).x();
  double width = plotHiX - plotLoX;
  QRectF r(plotLoX, plotArea.top(), width, plotArea.height());
  m_filterPassbandRectB->setRect(r);
}

void
QtPanadapter::plot(const ComplexSamplesMax* timeSeriesData,
    uint32_t length,
    uint32_t sampleRate,
    int64_t centreFrequency,
    bool shuffle)
{
  if (centreFrequency != 14190000) {
    bool pb = true;
  }
  RealSamplesMax spectrum(length);
  powerSpectrum(*timeSeriesData, length, spectrum);
  plot(&spectrum, sampleRate, centreFrequency, shuffle);
}

void
QtPanadapter::plot(
    const RealSamplesMax* spectrumData,
    uint32_t sampleRate,
    int64_t centreFrequency,
    bool shuffle
  )
{
  qreal plotX = static_cast<qreal>(centreFrequency) - (static_cast<qreal>(sampleRate) / 2);
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
QtPanadapter::powerSpectrum(const ComplexSamplesMax& timeSeries, uint32_t timeSeriesLength, RealSamplesMax& spectrumOut)
{
  ComplexSamplesMax windowed(timeSeriesLength);
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

  auto rbw = static_cast<float>(timeSeriesLength);
  auto normalization = static_cast<float>(timeSeriesLength);
  for (uint32_t i = 0; i < timeSeriesLength; i++) {
    float mag_sq = std::norm(fftOut.at(i));
    spectrumOut.at(i) = 10.0f * std::log10(mag_sq / (rbw * normalization));
  }
}