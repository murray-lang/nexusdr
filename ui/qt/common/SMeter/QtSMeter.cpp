//
// Created by murray on 19/2/26.
//

#include "QtSMeter.h"

#include "ui/qt/common/ScaleRenderer/ScaleLayoutHelpers.h"
#include "ui/qt/common/ScaleRenderer/ScaleModelHelpers.h"
#include "ui/qt/common/ScaleRenderer/ScaleRenderer.h"
#include "ui/qt/common/SMeter/SMeterTickProvider.h"

#include <QEvent>
#include <QFontMetrics>
#include <QPainter>
#include <QPen>
#include <algorithm>
#include <cmath>

QtSMeter::QtSMeter(QWidget* parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_StyledBackground, true);

  // Important: allow the widget to grow if the UI gives it room.
  // Previously this was (Expanding, Fixed) for horizontal, which prevents filling 300x100.
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void QtSMeter::setBarThickness(int px)
{
  m_barThicknessPx = std::clamp(px, 4, 200);
  updateGeometry();
  update();
}

void QtSMeter::setShowScale(bool on)
{
  if (m_showScale == on) {
    return;
  }
  m_showScale = on;
  updateGeometry();
  update();
}

void QtSMeter::setScalePlacement(ScalePlacement placement)
{
  if (m_scalePlacement == placement) {
    return;
  }
  m_scalePlacement = placement;
  updateGeometry();
  update();
}

void QtSMeter::setScalePlacementText(const QString& text)
{
  if (m_scalePlacementText == text) {
    return;
  }
  m_scalePlacementText = text;
  updateGeometry();
  update();
}

void QtSMeter::setOverS9MaxDb(int db)
{
  db = std::clamp(db, 0, 120);
  if (m_overS9MaxDb == db) {
    return;
  }
  m_overS9MaxDb = db;

  // Keep range consistent with reference if user already called setReferenceDbFs().
  // (If not, this is harmless.)
  setReferenceDbFs(m_s9DbFs, static_cast<float>(m_overS9MaxDb));
}

void QtSMeter::setRangeDbFs(float minDbFs, float maxDbFs)
{
  if (!std::isfinite(minDbFs) || !std::isfinite(maxDbFs)) {
    return;
  }
  if (maxDbFs <= minDbFs) {
    maxDbFs = minDbFs + 1.0f;
  }
  m_minDbFs = minDbFs;
  m_maxDbFs = maxDbFs;
  update();
}

void QtSMeter::setReferenceDbFs(float referenceDbFs, float overS9MaxDb)
{
  if (!std::isfinite(referenceDbFs)) {
    return;
  }

  m_s9DbFs = referenceDbFs;
  m_overS9MaxDb = std::clamp(static_cast<int>(std::lround(overS9MaxDb)), 0, 120);

  const float minDbFs = m_s9DbFs - 54.0f; // S0
  const float maxDbFs = m_s9DbFs + static_cast<float>(m_overS9MaxDb);

  setRangeDbFs(minDbFs, maxDbFs);
  setZoneThresholdsDbFs(m_s9DbFs, m_s9DbFs + 20.0f);
}

void QtSMeter::setZoneThresholdsDbFs(float yellowFromDbFs, float redFromDbFs)
{
  if (!std::isfinite(yellowFromDbFs) || !std::isfinite(redFromDbFs)) {
    return;
  }

  m_yellowFromDbFs = yellowFromDbFs;
  m_redFromDbFs = redFromDbFs;

  if (m_redFromDbFs < m_yellowFromDbFs) {
    std::swap(m_redFromDbFs, m_yellowFromDbFs);
  }
  update();
}

void QtSMeter::setOrientation(Qt::Orientation o)
{
  if (m_orientation == o) {
    return;
  }
  m_orientation = o;

  // Allow growth in both directions. Layout decides final size.
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  updateGeometry();
  update();
}

void QtSMeter::setReading(float rssiDbFs, std::optional<float> agcGainDb)
{
  if (!std::isfinite(rssiDbFs)) {
    return;
  }

  m_lastValueDbFs = rssiDbFs;
  updateToolTip(rssiDbFs, agcGainDb);

  update();
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

// -----------------------
// Theme property setters
// -----------------------
void QtSMeter::setBackgroundColor(const QColor& c) { m_backgroundColor = c; update(); }
void QtSMeter::setBorderColor(const QColor& c) { m_borderColor = c; update(); }
void QtSMeter::setBorderWidth(int px) { m_borderWidth = std::clamp(px, 0, 20); updateGeometry(); update(); }

void QtSMeter::setBarOkColor(const QColor& c) { m_barOkColor = c; update(); }
void QtSMeter::setBarWarnColor(const QColor& c) { m_barWarnColor = c; update(); }
void QtSMeter::setBarAlarmColor(const QColor& c) { m_barAlarmColor = c; update(); }
void QtSMeter::setBarTrackColor(const QColor& c) { m_barTrackColor = c; update(); }

void QtSMeter::setTickColor(const QColor& c) { m_tickColor = c; update(); }
void QtSMeter::setLabelColor(const QColor& c) { m_labelColor = c; update(); }

void QtSMeter::setMajorTickPx(int px) { m_majorTickPx = std::clamp(px, 0, 50); updateGeometry(); update(); }
void QtSMeter::setMinorTickPx(int px) { m_minorTickPx = std::clamp(px, 0, 50); updateGeometry(); update(); }
void QtSMeter::setTickToLabelGapPx(int px) { m_tickToLabelGapPx = std::clamp(px, 0, 50); updateGeometry(); update(); }
void QtSMeter::setHideLabelsWhenTight(bool on) { m_hideLabelsWhenTight = on; updateGeometry(); update(); }

void QtSMeter::setEndPaddingPx(int px) { m_endPaddingPx = std::clamp(px, 0, 200); update(); }
void QtSMeter::setBarInsetPx(int px) { m_barInsetPx = std::clamp(px, 0, 20); update(); }
void QtSMeter::setScalePadPx(int px)
{
  px = std::clamp(px, 0, 50);
  if (m_scalePadPx == px) {
    return;
  }
  m_scalePadPx = px;
  updateGeometry();
  update();
}

// -----------------------
// Placement helpers
// -----------------------
QtSMeter::ScalePlacement QtSMeter::parsePlacementTextOr(ScalePlacement fallback) const
{
  const QString s = m_scalePlacementText.trimmed().toLower();
  if (s.isEmpty()) {
    return fallback;
  }

  if (s == "auto") return ScalePlacement::Auto;
  if (s == "top") return ScalePlacement::Top;
  if (s == "bottom") return ScalePlacement::Bottom;
  if (s == "left") return ScalePlacement::Left;
  if (s == "right") return ScalePlacement::Right;

  return fallback;
}

QtSMeter::ScalePlacement QtSMeter::effectiveScalePlacement() const
{
  const ScalePlacement requested = parsePlacementTextOr(m_scalePlacement);

  if (m_orientation == Qt::Horizontal) {
    switch (requested) {
      case ScalePlacement::Top:
      case ScalePlacement::Bottom:
        return requested;
      case ScalePlacement::Auto:
      default:
        return ScalePlacement::Bottom;
    }
  }

  switch (requested) {
    case ScalePlacement::Left:
    case ScalePlacement::Right:
      return requested;
    case ScalePlacement::Auto:
    default:
      return ScalePlacement::Left;
  }
}

double QtSMeter::axisRatioForPlacement(ScalePlacement p) const
{
  // Put axis line close to the bar-adjacent edge so the scale "hugs" the bar visually.
  // (Labels/ticks then extend away from the bar.)
  constexpr double kNearEdge = 0.0;
  constexpr double kFarEdge  = 1.0 - kNearEdge;

  if (m_orientation == Qt::Horizontal) {
    return (p == ScalePlacement::Top) ? kFarEdge : kNearEdge;
  }
  return (p == ScalePlacement::Left) ? kFarEdge : kNearEdge;
}

int QtSMeter::tickDirectionForPlacement(ScalePlacement p) const
{
  // 0 = TowardNearSide, 1 = TowardFarSide, 2 = Both
  if (p == ScalePlacement::Top || p == ScalePlacement::Right) {
    return 1;
  }
  return 0;
}

// -----------------------
// Geometry
// -----------------------
int QtSMeter::scaleBandThicknessPx() const
{
  if (!m_showScale) {
    return 0;
  }

  const QFontMetrics fm(font());
  const int textH = fm.height();
  const int pad = 2 * 2;
  const int tick = std::max(m_majorTickPx, m_minorTickPx);
  const int gap = m_tickToLabelGapPx;

  const int lane = textH + pad + 2;
  const int thickness = tick + gap + lane + 2;

  return std::clamp(thickness, 14, 80);
}

QRect QtSMeter::barRect(const QRect& outer) const
{
  if (!m_showScale) {
    return outer;
  }

  const int scaleT = scaleBandThicknessPx();
  const ScalePlacement place = effectiveScalePlacement();

  if (m_orientation == Qt::Horizontal) {
    // Bar is fixed thickness; scale consumes the remaining height.
    const int availForBar = std::max(0, outer.height() - scaleT);
    const int hBar = std::clamp(m_barThicknessPx, 0, availForBar);

    if (place == ScalePlacement::Top) {
      // scale at top -> bar at bottom
      return QRect(outer.left(), outer.bottom() - hBar + 1, outer.width(), hBar);
    }
    // Bottom (default): bar at top
    return QRect(outer.left(), outer.top(), outer.width(), hBar);
  }

  // Vertical: bar is fixed thickness; scale consumes remaining width.
  const int availForBar = std::max(0, outer.width() - scaleT);
  const int wBar = std::clamp(m_barThicknessPx, 0, availForBar);

  if (place == ScalePlacement::Right) {
    // scale at right -> bar at left
    return QRect(outer.left(), outer.top(), wBar, outer.height());
  }
  // Left (default) -> bar at right
  return QRect(outer.right() - wBar + 1, outer.top(), wBar, outer.height());
}

QRect QtSMeter::scaleRect(const QRect& outer) const
{
  if (!m_showScale) {
    return QRect();
  }

  const ScalePlacement place = effectiveScalePlacement();
  const QRect b = barRect(outer);

  // IMPORTANT: use the painted bar fill edge, not the bar widget rect edge
  const QRect inner = barInnerFillRect(b);

  const int gap = std::max(0, m_scalePadPx);

  if (m_orientation == Qt::Horizontal) {
    if (place == ScalePlacement::Top) {
      // Scale is above the bar, ending gap pixels before the *fill* begins
      const int bottom = std::min(outer.bottom(), inner.top() - gap - 1);
      const int h = std::max(0, bottom - outer.top() + 1);
      return QRect(outer.left(), outer.top(), outer.width(), h);
    }

    // Bottom: scale is below the bar, starting gap pixels after the *fill* ends
    const int y = std::max(outer.top(), inner.bottom() + gap + 1);
    const int h = std::max(0, outer.bottom() - y + 1);
    return QRect(outer.left(), y, outer.width(), h);
  }

  // Vertical
  if (place == ScalePlacement::Right) {
    // Scale is to the right of the bar, starting gap pixels after the *fill* ends
    const int x = std::max(outer.left(), inner.right() + gap + 1);
    const int w = std::max(0, outer.right() - x + 1);
    return QRect(x, outer.top(), w, outer.height());
  }

  // Left: scale is to the left of the bar, ending gap pixels before the *fill* begins
  const int right = std::min(outer.right(), inner.left() - gap - 1);
  const int w = std::max(0, right - outer.left() + 1);
  return QRect(outer.left(), outer.top(), w, outer.height());
}

QRect QtSMeter::barInnerFillRect(const QRect& barR) const
{
  // Border inset applies in both directions.
  const int bw = std::max(0, m_borderWidth);
  QRect r = barR.adjusted(bw, bw, -bw, -bw);

  // IMPORTANT:
  // barInsetPx should only shorten the bar at the ENDS (axis direction),
  // not reduce its thickness (perpendicular direction), otherwise the painted
  // bar becomes much thinner than m_barThicknessPx and looks like a big "gap".
  const int inset = std::max(0, m_barInsetPx);

  if (m_orientation == Qt::Horizontal) {
    // shorten left/right only
    r = r.adjusted(inset, 0, -inset, 0);
  } else {
    // shorten top/bottom only
    r = r.adjusted(0, inset, 0, -inset);
  }
  return r;
}

// -----------------------
// Mapping helpers
// -----------------------
double QtSMeter::clamp01(double t) const
{
  if (!std::isfinite(t)) return 0.0;
  return std::clamp(t, 0.0, 1.0);
}

double QtSMeter::valueToT(double valueDbFs) const
{
  const double lo = static_cast<double>(m_minDbFs);
  const double hi = static_cast<double>(m_maxDbFs);
  const double span = (std::abs(hi - lo) < 1e-12) ? 1.0 : (hi - lo);
  return clamp01((static_cast<double>(valueDbFs) - lo) / span);
}

int QtSMeter::tToPixelAlongAxis(double t, const QRect& axisSpanRect) const
{
  t = clamp01(t);

  if (m_orientation == Qt::Horizontal) {
    const int x0 = axisSpanRect.left();
    const int x1 = axisSpanRect.right();
    const double x = static_cast<double>(x0) + (static_cast<double>(x1 - x0) * t);
    return static_cast<int>(std::lround(x));
  }

  const int y0 = axisSpanRect.bottom();
  const int y1 = axisSpanRect.top();
  const double y = static_cast<double>(y0) + (static_cast<double>(y1 - y0) * t);
  return static_cast<int>(std::lround(y));
}

// -----------------------
// Painting
// -----------------------
void QtSMeter::paintBackgroundAndBorder(QPainter& p, const QRect& outer) const
{
  p.save();
  p.setRenderHint(QPainter::Antialiasing, false);

  p.fillRect(outer, m_backgroundColor);

  if (m_borderWidth > 0) {
    QPen pen(m_borderColor);
    pen.setCosmetic(true);
    pen.setWidth(m_borderWidth);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawRect(outer.adjusted(0, 0, -1, -1));
  }

  p.restore();
}

void QtSMeter::paintBar(QPainter& p, const QRect& barR) const
{
  p.save();
  p.setRenderHint(QPainter::Antialiasing, false);

  const QRect inner = barInnerFillRect(barR);
  if (inner.isEmpty()) {
    p.restore();
    return;
  }

  QRect axisSpan = inner;
  if (m_orientation == Qt::Horizontal) {
    axisSpan.setLeft(axisSpan.left() + m_endPaddingPx);
    axisSpan.setRight(axisSpan.right() - m_endPaddingPx);
  } else {
    axisSpan.setTop(axisSpan.top() + m_endPaddingPx);
    axisSpan.setBottom(axisSpan.bottom() - m_endPaddingPx);
  }

  if (axisSpan.isEmpty()) {
    p.restore();
    return;
  }
  // Track background ONLY in usable span (so it doesn't “stick out” past the bar ends)
  p.fillRect(axisSpan, m_barTrackColor);

  const int pxYellow = tToPixelAlongAxis(valueToT(m_yellowFromDbFs), axisSpan);
  const int pxRed = tToPixelAlongAxis(valueToT(m_redFromDbFs), axisSpan);
  const int pxValue = tToPixelAlongAxis(valueToT(m_lastValueDbFs), axisSpan);

  auto fillSegmentH = [&](int x0, int x1, const QColor& c) {
    if (x1 <= x0) return;
    p.fillRect(QRect(x0, inner.top(), x1 - x0, inner.height()), c);
  };

  auto fillSegmentV = [&](int y0, int y1, const QColor& c) {
    if (y1 <= y0) return;
    p.fillRect(QRect(inner.left(), y0, inner.width(), y1 - y0), c);
  };

  if (m_orientation == Qt::Horizontal) {
    const int xStart = axisSpan.left();
    const int xEndValue = std::clamp(pxValue, axisSpan.left(), axisSpan.right());

    const int xYellow = std::clamp(pxYellow, axisSpan.left(), axisSpan.right());
    const int xRed = std::clamp(pxRed, axisSpan.left(), axisSpan.right());

    const int x1 = std::min(xEndValue, xYellow);
    const int x2 = std::min(std::max(xEndValue, xYellow), xRed);
    const int x3 = xEndValue;

    fillSegmentH(xStart, x1, m_barOkColor);
    if (xEndValue > xYellow) fillSegmentH(xYellow, x2, m_barWarnColor);
    if (xEndValue > xRed) fillSegmentH(xRed, x3, m_barAlarmColor);
  } else {
    const int yStart = axisSpan.bottom();
    const int yEndValue = std::clamp(pxValue, axisSpan.top(), axisSpan.bottom());

    const int yYellow = std::clamp(pxYellow, axisSpan.top(), axisSpan.bottom());
    const int yRed = std::clamp(pxRed, axisSpan.top(), axisSpan.bottom());

    const int yTopValue = std::min(yEndValue, yStart);
    const int yTopOk = std::min(yYellow, yStart);
    const int yTopWarn = std::min(yRed, yStart);

    const int yOkTop = std::max(yTopValue, yTopOk);
    fillSegmentV(yOkTop, yStart, m_barOkColor);

    if (yTopValue < yTopOk) {
      const int yWarnTop = std::max(yTopValue, yTopWarn);
      fillSegmentV(yWarnTop, yTopOk, m_barWarnColor);
    }

    if (yTopValue < yTopWarn) {
      fillSegmentV(yTopValue, yTopWarn, m_barAlarmColor);
    }
  }

  p.restore();
}

void QtSMeter::paintScale(QPainter& p,
                          const QRect& outer,
                          const QRect& barR,
                          const QRect& barFillRect,
                          const QRect& barInnerAxisSpan) const
{
  (void)barR;

  if (!m_showScale) {
    return;
  }

  p.save();

  const ScalePlacement place = effectiveScalePlacement();
  const int gap = std::max(0, m_scalePadPx);

  // Fixed thickness for the scale band (hugging band)
  const int bandT = scaleBandThicknessPx();

  QRect usedScaleR;

  if (m_orientation == Qt::Horizontal) {
    if (place == ScalePlacement::Top) {
      // Scale band is above the bar fill, with exact gap
      const int bottom = barFillRect.top() - 1 - gap;
      const int top = bottom - bandT + 1;
      usedScaleR = QRect(outer.left(), top, outer.width(), bandT);
    } else {
      // Bottom: scale band is below the bar fill, with exact gap
      const int top = barFillRect.bottom() + 1 + gap;
      usedScaleR = QRect(outer.left(), top, outer.width(), bandT);
    }

    // Clip to widget bounds
    usedScaleR = usedScaleR.intersected(outer);
  } else {
    if (place == ScalePlacement::Right) {
      // Scale band is to the right of the bar fill
      const int left = barFillRect.right() + 1 + gap;
      usedScaleR = QRect(left, outer.top(), bandT, outer.height());
    } else {
      // Left: scale band is to the left of the bar fill
      const int right = barFillRect.left() - 1 - gap;
      const int left = right - bandT + 1;
      usedScaleR = QRect(left, outer.top(), bandT, outer.height());
    }

    usedScaleR = usedScaleR.intersected(outer);
  }

  if (usedScaleR.isEmpty()) {
    p.restore();
    return;
  }

  // Axis span inside the scale band should match the bar’s usable span along-axis.
  QRect axisSpanInScale = usedScaleR;
  if (m_orientation == Qt::Horizontal) {
    axisSpanInScale.setLeft(barInnerAxisSpan.left());
    axisSpanInScale.setRight(barInnerAxisSpan.right());
  } else {
    axisSpanInScale.setTop(barInnerAxisSpan.top());
    axisSpanInScale.setBottom(barInnerAxisSpan.bottom());
  }

  ScaleStyle s;
  s.tickColor = m_tickColor;
  s.labelColor = m_labelColor;
  s.majorTickPx = m_majorTickPx;
  s.minorTickPx = m_minorTickPx;
  s.tickToLabelGapPx = m_tickToLabelGapPx;
  s.hideLabelsWhenTight = m_hideLabelsWhenTight;
  s.textOrientation = ScaleStyle::TextOrientation::HorizontalAlways;

  // Keep labels away from the bar (your preference)
  if (place == ScalePlacement::Top || place == ScalePlacement::Right) {
    s.placement = ScaleStyle::PlacementPolicy::PreferFarSide;
  } else {
    s.placement = ScaleStyle::PlacementPolicy::PreferNearSide;
  }

  // Ticks go away from the bar
  s.tickDirection = static_cast<ScaleStyle::TickDirection>(tickDirectionForPlacement(place));

  // Start with a centered layout, then pin the axis line to the bar-adjacent edge.
  ScaleLayout layout = ScaleLayoutHelpers::makeCenteredLayout(
    usedScaleR,
    axisSpanInScale,
    m_orientation,
    m_endPaddingPx
  );

  if (m_orientation == Qt::Horizontal) {
    // Bottom: axis at TOP edge of band (tick top touches bar gap boundary)
    // Top:    axis at BOTTOM edge of band
    layout.axisPosPx = (place == ScalePlacement::Top) ? usedScaleR.bottom() : usedScaleR.top();
    layout.nearSideExtentPx = std::max(0, usedScaleR.bottom() - layout.axisPosPx);
    layout.farSideExtentPx  = std::max(0, layout.axisPosPx - usedScaleR.top());
  } else {
    // Left:  axis at RIGHT edge; Right: axis at LEFT edge
    layout.axisPosPx = (place == ScalePlacement::Right) ? usedScaleR.left() : usedScaleR.right();
    layout.nearSideExtentPx = std::max(0, layout.axisPosPx - usedScaleR.left());
    layout.farSideExtentPx  = std::max(0, usedScaleR.right() - layout.axisPosPx);
  }

  ScaleModel model = ScaleModelHelpers::makeModel(
    static_cast<double>(m_minDbFs),
    static_cast<double>(m_maxDbFs),
    m_orientation,
    layout
  );

  SMeterTickProvider ticks(static_cast<double>(m_s9DbFs));
  ticks.setMinorsPerInterval(1);
  ticks.setOverS9MaxPlusDb(m_overS9MaxDb);

  ScaleRenderer renderer;
  renderer.paint(p, font(), s, model, layout, ticks);

  p.restore();
}

void QtSMeter::paintEvent(QPaintEvent* e)
{
  (void)e;

  QPainter p(this);

  const QRect outer = rect();
  if (outer.isEmpty()) {
    return;
  }

  paintBackgroundAndBorder(p, outer);

  const QRect barR = barRect(outer);
  paintBar(p, barR);

  if (!m_showScale) {
    return;
  }

  const QRect barFillRect = barInnerFillRect(barR);

  // Provide a consistent axis span for the scale to align against:
  QRect barAxisSpan = barFillRect;

  if (m_orientation == Qt::Horizontal) {
    barAxisSpan.setLeft(barAxisSpan.left() + m_endPaddingPx);
    barAxisSpan.setRight(barAxisSpan.right() - m_endPaddingPx);
  } else {
    barAxisSpan.setTop(barAxisSpan.top() + m_endPaddingPx);
    barAxisSpan.setBottom(barAxisSpan.bottom() - m_endPaddingPx);
  }

  paintScale(p, outer, barR, barFillRect, barAxisSpan);
}

bool QtSMeter::event(QEvent* e)
{
  switch (e->type()) {
    case QEvent::StyleChange:
    case QEvent::FontChange:
    case QEvent::PaletteChange:
    case QEvent::DynamicPropertyChange:
      updateGeometry();
      update();
      break;
    default:
      break;
  }
  return QWidget::event(e);
}

QSize QtSMeter::sizeHint() const
{
  const int s = scaleBandThicknessPx();

  if (m_orientation == Qt::Horizontal) {
    return QSize(240, std::max(8, m_barThicknessPx) + s);
  }
  return QSize(std::max(8, m_barThicknessPx) + s, 180);
}

QSize QtSMeter::minimumSizeHint() const
{
  const int s = scaleBandThicknessPx();

  if (m_orientation == Qt::Horizontal) {
    return QSize(80, std::max(8, m_barThicknessPx) + s);
  }
  return QSize(std::max(12, m_barThicknessPx) + s, 80);
}