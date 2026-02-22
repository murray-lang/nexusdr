//
// Created by murray on 19/2/26.
//
#include "ScaleRenderer.h"

#include <QFontMetrics>
#include <QPen>

static bool isFinite(double v)
{
  return std::isfinite(v);
}

static QRect clampRectToRect(QRect r, const QRect& bounds)
{
  if (r.width() > bounds.width()) {
    r.setLeft(bounds.left());
    r.setWidth(bounds.width());
  } else {
    if (r.left() < bounds.left()) r.moveLeft(bounds.left());
    if (r.right() > bounds.right()) r.moveRight(bounds.right());
  }

  if (r.height() > bounds.height()) {
    r.setTop(bounds.top());
    r.setHeight(bounds.height());
  } else {
    if (r.top() < bounds.top()) r.moveTop(bounds.top());
    if (r.bottom() > bounds.bottom()) r.moveBottom(bounds.bottom());
  }

  return r;
}

double ScaleRenderer::safeSpan(double a, double b)
{
  const double s = b - a;
  return (std::abs(s) < 1e-12) ? 1.0 : s;
}

double ScaleRenderer::clamp01(double t)
{
  if (!isFinite(t)) return 0.0;
  return std::clamp(t, 0.0, 1.0);
}

double ScaleRenderer::valueToT(double value, const ScaleModel& model)
{
  const double lo = model.min;
  const double hi = model.max;
  const double span = safeSpan(lo, hi);
  return clamp01((value - lo) / span);
}

int ScaleRenderer::tToPixel(double t, const ScaleModel& model, const QRect& axisSpanRect)
{
  t = clamp01(t);

  if (model.orientation == Qt::Horizontal) {
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

bool ScaleRenderer::rectsOverlapAlongAxis(const QRect& a, const QRect& b, Qt::Orientation o, int gapPx)
{
  if (o == Qt::Horizontal) {
    const int a0 = a.left();
    const int a1 = a.right();
    const int b0 = b.left();
    const int b1 = b.right();
    return !(b0 > a1 + gapPx || a0 > b1 + gapPx);
  } else {
    const int a0 = a.top();
    const int a1 = a.bottom();
    const int b0 = b.top();
    const int b1 = b.bottom();
    return !(b0 > a1 + gapPx || a0 > b1 + gapPx);
  }
}

QRect ScaleRenderer::labelRectForTick(const QFontMetrics& fm,
                                     const ScaleStyle& style,
                                     const ScaleModel& model,
                                     const ScaleLayout& layout,
                                     int axisPixel,
                                     const QString& text,
                                     bool farSide) const
{
  const int textW = fm.horizontalAdvance(text);
  const int textH = fm.height();

  const int pad = std::max(0, style.textPaddingPx);
  const int boxW = textW + 2 * pad;
  const int boxH = textH + 2 * pad;

  if (model.orientation == Qt::Horizontal) {
    const int cx = axisPixel;
    const int x = cx - (boxW / 2);

    // far side = above, near side = below
    if (farSide) {
      const int y = layout.axisPosPx - style.majorTickPx - style.tickToLabelGapPx - boxH;
      return QRect(x, y, boxW, boxH);
    } else {
      const int y = layout.axisPosPx + style.majorTickPx + style.tickToLabelGapPx;
      return QRect(x, y, boxW, boxH);
    }
  }

  // Vertical with horizontal labels
  const int cy = axisPixel;
  const int y = cy - (boxH / 2);

  // far side = right, near side = left
  if (farSide) {
    const int x = layout.axisPosPx + style.majorTickPx + style.tickToLabelGapPx;
    return QRect(x, y, boxW, boxH);
  } else {
    const int x = layout.axisPosPx - style.majorTickPx - style.tickToLabelGapPx - boxW;
    return QRect(x, y, boxW, boxH);
  }
}

void ScaleRenderer::drawTick(QPainter& p,
                             const ScaleStyle& style,
                             const ScaleModel& model,
                             const ScaleLayout& layout,
                             int axisPixel,
                             bool major) const
{
  const int len = std::max(0, major ? style.majorTickPx : style.minorTickPx);

  if (model.orientation == Qt::Horizontal) {
    const int x = axisPixel;
    const int y = layout.axisPosPx;

    auto drawDown = [&]() { p.drawLine(QPoint(x, y), QPoint(x, y + len)); };
    auto drawUp = [&]() { p.drawLine(QPoint(x, y), QPoint(x, y - len)); };

    switch (style.tickDirection) {
      case ScaleStyle::TickDirection::TowardFarSide:
        drawUp();
        break;
      case ScaleStyle::TickDirection::Both:
        drawUp();
        drawDown();
        break;
      case ScaleStyle::TickDirection::TowardNearSide:
      default:
        drawDown();
        break;
    }
    return;
  }

  // Vertical
  const int y = axisPixel;
  const int x = layout.axisPosPx;

  auto drawLeft = [&]() { p.drawLine(QPoint(x, y), QPoint(x - len, y)); };
  auto drawRight = [&]() { p.drawLine(QPoint(x, y), QPoint(x + len, y)); };

  switch (style.tickDirection) {
    case ScaleStyle::TickDirection::TowardFarSide:
      drawRight();
      break;
    case ScaleStyle::TickDirection::Both:
      drawLeft();
      drawRight();
      break;
    case ScaleStyle::TickDirection::TowardNearSide:
    default:
      drawLeft();
      break;
  }
}

void ScaleRenderer::drawLabel(QPainter& p,
                              const QFont& font,
                              const ScaleStyle& style,
                              const QRect& r,
                              const QString& text) const
{
  p.save();
  p.setFont(font);
  p.setPen(style.labelColor);

  const int pad = std::max(0, style.textPaddingPx);
  const QRect inner = r.adjusted(pad, pad, -pad, -pad);

  p.drawText(inner, Qt::AlignCenter, text);
  p.restore();
}

int ScaleRenderer::idealThicknessPx(const QFontMetrics& fm,
                                   const ScaleStyle& style,
                                   bool allowLabels) const
{
  const int tickLen = std::max(style.majorTickPx, style.minorTickPx);
  const int gap = std::max(0, style.tickToLabelGapPx);

  if (!allowLabels) {
    return tickLen + 2;
  }

  const int textH = fm.height() + 2 * std::max(0, style.textPaddingPx);
  return tickLen + gap + textH + 2;
}

void ScaleRenderer::paint(QPainter& p,
                          const QFont& font,
                          const ScaleStyle& style,
                          const ScaleModel& model,
                          const ScaleLayout& layout,
                          const IScaleTickProvider& tickProvider) const
{
  if (layout.scaleRect.isEmpty() || layout.axisSpanRect.isEmpty()) {
    return;
  }

  if (!isFinite(model.min) || !isFinite(model.max)) {
    return;
  }

  p.save();
  p.setRenderHint(QPainter::Antialiasing, false);
  p.setFont(font);

  QPen tickPen(style.tickColor);
  tickPen.setCosmetic(true);
  p.setPen(tickPen);

  p.setClipRect(layout.scaleRect);

  QVector<ScaleTick> ticks;
  tickProvider.buildTicks(model, ticks);
  if (ticks.isEmpty()) {
    p.restore();
    return;
  }

  std::sort(ticks.begin(), ticks.end(), [](const ScaleTick& a, const ScaleTick& b) {
    if (a.value == b.value) return static_cast<int>(a.kind) < static_cast<int>(b.kind);
    return a.value < b.value;
  });

  const QFontMetrics fm(font);
  const int minLane = std::max(layout.nearSideExtentPx, layout.farSideExtentPx);
  const int textH = fm.height() + 2 * std::max(0, style.textPaddingPx);

  bool labelsAllowed = true;
  if (style.hideLabelsWhenTight) {
    if (minLane < textH) {
      labelsAllowed = false;
    }
  }

  QRect lastNear;
  QRect lastFar;
  bool haveLastNear = false;
  bool haveLastFar = false;
  bool alternateFlip = false;

  // ticks first
  for (const auto& t : ticks) {
    const double tt = valueToT(t.value, model);
    const int axisPx = tToPixel(tt, model, layout.axisSpanRect);

    if (model.orientation == Qt::Horizontal) {
      if (axisPx < layout.axisSpanRect.left() || axisPx > layout.axisSpanRect.right()) continue;
    } else {
      if (axisPx < layout.axisSpanRect.top() || axisPx > layout.axisSpanRect.bottom()) continue;
    }

    drawTick(p, style, model, layout, axisPx, t.kind == ScaleTick::Kind::Major);
  }

  if (!labelsAllowed) {
    p.restore();
    return;
  }

  for (const auto& t : ticks) {
    if (!t.hasLabel || t.label.isEmpty()) continue;
    if (t.kind != ScaleTick::Kind::Major) continue;

    const double tt = valueToT(t.value, model);
    const int axisPx = tToPixel(tt, model, layout.axisSpanRect);

    if (model.orientation == Qt::Horizontal) {
      if (axisPx < layout.axisSpanRect.left() || axisPx > layout.axisSpanRect.right()) continue;
    } else {
      if (axisPx < layout.axisSpanRect.top() || axisPx > layout.axisSpanRect.bottom()) continue;
    }

    QRect nearRect = labelRectForTick(fm, style, model, layout, axisPx, t.label, /*farSide=*/false);
    QRect farRect  = labelRectForTick(fm, style, model, layout, axisPx, t.label, /*farSide=*/true);

    // NEW: clamp candidate rects into the band instead of rejecting them for minor overflow.
    nearRect = clampRectToRect(nearRect, layout.scaleRect);
    farRect  = clampRectToRect(farRect,  layout.scaleRect);

    auto overlapsNear = [&]() -> bool {
      return haveLastNear && rectsOverlapAlongAxis(lastNear, nearRect, model.orientation, 2);
    };
    auto overlapsFar = [&]() -> bool {
      return haveLastFar && rectsOverlapAlongAxis(lastFar, farRect, model.orientation, 2);
    };

    const bool nearOk = !overlapsNear();
    const bool farOk  = !overlapsFar();

    bool chooseFar = false;
    bool place = false;

    switch (style.placement) {
      case ScaleStyle::PlacementPolicy::PreferNearSide:
        if (nearOk) { chooseFar = false; place = true; }
        else if (farOk) { chooseFar = true; place = true; }
        break;

      case ScaleStyle::PlacementPolicy::PreferFarSide:
        if (farOk) { chooseFar = true; place = true; }
        else if (nearOk) { chooseFar = false; place = true; }
        break;

      case ScaleStyle::PlacementPolicy::Alternate:
        if (nearOk && farOk) {
          chooseFar = alternateFlip;
          alternateFlip = !alternateFlip;
          place = true;
        } else if (nearOk) {
          chooseFar = false; place = true;
        } else if (farOk) {
          chooseFar = true; place = true;
        }
        break;

      case ScaleStyle::PlacementPolicy::BestFit:
      default:
        if (nearOk && farOk) {
          if (layout.farSideExtentPx != layout.nearSideExtentPx) {
            chooseFar = (layout.farSideExtentPx > layout.nearSideExtentPx);
          } else {
            chooseFar = alternateFlip;
            alternateFlip = !alternateFlip;
          }
          place = true;
        } else if (nearOk) {
          chooseFar = false; place = true;
        } else if (farOk) {
          chooseFar = true; place = true;
        }
        break;
    }

    if (!place) continue;

    const QRect& r = chooseFar ? farRect : nearRect;
    drawLabel(p, font, style, r, t.label);

    if (chooseFar) { lastFar = r; haveLastFar = true; }
    else { lastNear = r; haveLastNear = true; }
  }

  p.restore();
}