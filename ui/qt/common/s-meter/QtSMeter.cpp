//
// Created by murray on 18/2/26.
//

#include "QtSMeter.h"

#include "SMeterBarWidget.h"
#include "SMeterScaleDraw.h"

#include <QVBoxLayout>
#include <QFont>
#include <QList>
#include <cmath>
#include <algorithm>

#include <qwt_abstract_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_div.h>

#include "ui/qt/common/QWidgetPropertySetter.h"

static QList<double> makeMajorTicks(double lower, double upper, double majorStep)
{
  QList<double> ticks;
  const double first = std::ceil(lower / majorStep) * majorStep;

  for (double v = first; v <= upper + 1e-9; v += majorStep) {
    ticks.append(v);
  }
  if (ticks.isEmpty() || ticks.first() > lower + 1e-9) {
    ticks.prepend(lower);
  }
  if (ticks.last() < upper - 1e-9) {
    ticks.append(upper);
  }
  return ticks;
}

static QList<double> makeSMajorTicks(double lower, double upper, double s9DbFs)
{
  QList<double> ticks;

  // Below S9: 6 dB per S-unit (S1..S9)
  for (int s = 1; s <= 9; ++s) {
    const double v = s9DbFs + (static_cast<double>(s) - 9.0) * 6.0;
    if (v >= lower - 1e-9 && v <= upper + 1e-9) {
      ticks.append(v);
    }
  }

  // Above S9: +10, +20, +30 ... (10 dB steps)
  for (int plus = 10; plus <= 60; plus += 10) { // cap at +60 for now
    const double v = s9DbFs + static_cast<double>(plus);
    if (v >= lower - 1e-9 && v <= upper + 1e-9) {
      ticks.append(v);
    }
  }

  std::sort(ticks.begin(), ticks.end());

  // Ensure at least endpoints if everything fell outside
  if (ticks.isEmpty()) {
    ticks.append(lower);
    ticks.append(upper);
  }

  return ticks;
}

QtSMeter::QtSMeter(QWidget* parent)
  : QWidget(parent)
  , m_pScale(nullptr)
  , m_pBar(nullptr)
  , m_orientation(Qt::Horizontal)
  , m_minDbFs(-114.0f)
  , m_maxDbFs(-20.0f)
  , m_yellowFromDbFs(-50.0f)
  , m_redFromDbFs(-35.0f)
  , m_lastValueDbFs(-120.0f)
  , m_majorStepDb(10.0f)
  , m_minorTicksPerMajor(4)
  , m_s9DbFs(-60.0f)
{
  setAttribute(Qt::WA_StyledBackground, true);

  QWidgetPropertySetter::setWidgetProperty(this, "role", "sMeter", false);

  auto* l = new QVBoxLayout(this);
  l->setContentsMargins(0, 0, 0, 0);
  l->setSpacing(0);

  m_pBar = new SMeterBarWidget(this);
  l->addWidget(m_pBar);

  m_pScale = new QwtScaleWidget(this);
  QWidgetPropertySetter::setWidgetProperty(m_pScale, "role", "s-meter-scale", false);
  m_pScale->setAlignment(QwtScaleDraw::BottomScale);
  m_pScale->setMinimumHeight(22);

  // Custom label formatting
  m_pScale->setScaleDraw(new SMeterScaleDraw());
  setScaleLabelsSUnits(m_s9DbFs);
  // m_pScale->setStyleSheet("color: rgb(220,220,220); background-color: red;");

  l->addWidget(m_pScale);

  // apply initial settings
  m_pBar->setRange(m_minDbFs, m_maxDbFs);
  m_pBar->setThresholds(m_yellowFromDbFs, m_redFromDbFs);
  m_pBar->setValue(m_lastValueDbFs);

  setOrientation(Qt::Horizontal);

  constexpr int kEndPadPx = 12;
  // Bar fill is additionally inset by:
  // rect().adjusted(1,1,-1,-1)  -> 1px
  // then inner = r.adjusted(2,2,-2,-2) -> +2px
  // Total fixed inset along the axis direction = 3px on each end.
  constexpr int kBarFixedInsetPx = 3;

  m_pBar->setEndPadding(kEndPadPx);

  // Ask Qwt to keep first/last ticks away from the edges so labels can fit.
  // This makes the scale's "usable axis span" closer to what we draw in the bar.
  m_pScale->setBorderDist(kEndPadPx, kEndPadPx);
  // Match the scale's usable span to the bar's inner fill span.
  const int kTotalAxisInsetPx = kEndPadPx + kBarFixedInsetPx;
  m_pScale->setBorderDist(kTotalAxisInsetPx, kTotalAxisInsetPx);

  // Optional; sometimes helps keep things consistent:
  m_pScale->setMargin(0);

  applyScale();
  QWidgetPropertySetter::repolishRecursively(this);
}

int QtSMeter::thickness() const
{
  return m_pBar ? m_pBar->thickness() : 0;
}

void QtSMeter::setThickness(int px)
{
  if (m_pBar) {
    m_pBar->setThickness(px);
  }
}

void QtSMeter::setRangeDbFs(float minDbFs, float maxDbFs)
{
  if (maxDbFs <= minDbFs) {
    maxDbFs = minDbFs + 1.0f;
  }
  m_minDbFs = minDbFs;
  m_maxDbFs = maxDbFs;

  m_pBar->setRange(m_minDbFs, m_maxDbFs);
  applyScale();
}

void QtSMeter::setReferenceDbFs(float referenceDbFs, float overS9MaxDb)
{
  m_s9DbFs = referenceDbFs;

  // Make sure label formatter matches our reference
  setScaleLabelsSUnits(m_s9DbFs);

  // Choose a visually-uniform lower bound: one 6 dB step below S1 (i.e., "S0")
  const float minDbFs = m_s9DbFs - 54.0f;              // S0
  const float maxDbFs = m_s9DbFs + std::max(0.0f, overS9MaxDb);

  setRangeDbFs(minDbFs, maxDbFs);

  setZoneThresholdsDbFs(m_s9DbFs, m_s9DbFs + 20.0f);
}

void QtSMeter::setZoneThresholdsDbFs(float yellowFromDbFs, float redFromDbFs)
{
  m_yellowFromDbFs = yellowFromDbFs;
  m_redFromDbFs = redFromDbFs;
  if (m_redFromDbFs < m_yellowFromDbFs) {
    std::swap(m_redFromDbFs, m_yellowFromDbFs);
  }
  m_pBar->setThresholds(m_yellowFromDbFs, m_redFromDbFs);
}

void QtSMeter::setOrientation(Qt::Orientation o)
{
  m_orientation = o;
  if (m_pBar) {
    m_pBar->setOrientation(o);
  }

  if (!m_pScale) {
    return;
  }

  if (m_orientation == Qt::Horizontal) {
    m_pScale->setAlignment(QwtScaleDraw::BottomScale);
    m_pScale->setMinimumHeight(22);      // labels under bar
  } else {
    m_pScale->setAlignment(QwtScaleDraw::LeftScale);
    m_pScale->setMinimumWidth(28);       // room for labels beside bar
  }

  m_pScale->updateGeometry();
  m_pScale->update();
}

void QtSMeter::setScaleFontPointSize(int pt)
{
  pt = std::clamp(pt, 6, 24);
  QFont f = m_pScale->font();
  f.setPointSize(pt);
  m_pScale->setFont(f);
  m_pScale->updateGeometry();
  m_pScale->update();
}

void QtSMeter::setScaleTicks(float majorStepDb, int minorTicksPerMajor)
{
  m_majorStepDb = std::max(1.0f, majorStepDb);
  m_minorTicksPerMajor = std::clamp(minorTicksPerMajor, 0, 10);
  applyScale();
}

void QtSMeter::setScaleLabelsDbFs()
{
  if (auto* sd = dynamic_cast<SMeterScaleDraw*>(m_pScale->scaleDraw())) {
    sd->setModeDbFs();
    m_pScale->update();
  }
}

void QtSMeter::setScaleLabelsSUnits(float s9DbFs)
{
  m_s9DbFs = s9DbFs;

  if (auto* sd = dynamic_cast<SMeterScaleDraw*>(m_pScale->scaleDraw())) {
    sd->setModeSUnits(m_s9DbFs);
    m_pScale->update();
  }

  // Ticks depend on S9 reference, so regenerate divisions
  applyScale();
}

void QtSMeter::applyScale()
{
  if (!m_pScale) {
    return;
  }

  m_pScale->setAlignment(QwtScaleDraw::BottomScale);

  const auto lower = static_cast<double>(m_minDbFs);
  const auto upper = static_cast<double>(m_maxDbFs);

  // Major ticks at S-unit landmarks
  const QList<double> majorTicks = makeSMajorTicks(lower, upper, static_cast<double>(m_s9DbFs));

  // Minor ticks: subdivide each major interval into N parts (simple and consistent)
  const int minorsPerInterval = 1; // gives 1 minor ticks between majors
  QList<double> minorTicks;
  minorTicks.append(lower);
  minorTicks.append(upper);
  for (int i = 1; i < majorTicks.size(); ++i) {
    const double a = majorTicks[i - 1];
    const double b = majorTicks[i];
    const double step = (b - a) / static_cast<double>(minorsPerInterval + 1);
    for (int k = 1; k <= minorsPerInterval; ++k) {
      const double t = a + step * static_cast<double>(k);
      if (t > lower + 1e-9 && t < upper - 1e-9) {
        minorTicks.append(t);
      }
    }
  }

  QwtScaleDiv div(lower, upper);
  div.setTicks(QwtScaleDiv::MajorTick, majorTicks);
  div.setTicks(QwtScaleDiv::MediumTick, QList<double>{});
  div.setTicks(QwtScaleDiv::MinorTick, minorTicks);

  m_pScale->setScaleDiv(div);

  if (auto* sd = m_pScale->scaleDraw()) {
    sd->setTickLength(QwtScaleDiv::MinorTick, 3);
    sd->setTickLength(QwtScaleDiv::MediumTick, 5);
    sd->setTickLength(QwtScaleDiv::MajorTick, 9);
    sd->setSpacing(6);
  }

  m_pScale->updateGeometry();
  m_pScale->update();
}

void QtSMeter::updateToolTip(float rssiDbFs, const std::optional<float>& agcGainDb)
{
  QString tip;
  if (agcGainDb.has_value()) {
    tip = QString("RSSI: %1 dBFS\nAGC: %2 dB")
            .arg(rssiDbFs, 0, 'f', 1)
            .arg(*agcGainDb, 0, 'f', 1);
  } else {
    tip = QString("RSSI: %1 dBFS").arg(rssiDbFs, 0, 'f', 1);
  }

  if (tip != m_lastToolTip) {
    m_lastToolTip = tip;
    setToolTip(m_lastToolTip);
  }
}

void QtSMeter::setReading(float rssiDbFs, std::optional<float> agcGainDb)
{
  m_lastValueDbFs = rssiDbFs;
  updateToolTip(rssiDbFs, agcGainDb);

  m_pBar->setValue(m_lastValueDbFs);
}