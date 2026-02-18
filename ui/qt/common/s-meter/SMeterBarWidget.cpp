//
// Created by murray on 18/2/26.
//

#include "SMeterBarWidget.h"

#include <QPainter>
#include <QBrush>
#include <QColor>
#include <algorithm>
#include <QStyleOption>

#include "ui/qt/common/QWidgetPropertySetter.h"

#include "BarOverlayWidget.h"


SMeterBarWidget::SMeterBarWidget(QWidget* parent)
  : QWidget(parent)
  , m_orientation(Qt::Horizontal)
  , m_thicknessPx(14)
  , m_endPaddingPx(0)
  , m_minDbFs(-120.0f)
  , m_maxDbFs(-20.0f)
  , m_yellowFromDbFs(-50.0f)
  , m_redFromDbFs(-35.0f)
  , m_valueDbFs(-120.0f)
  , m_background(new QWidget(this))
  , m_colourSegmentOverlay(new BarOverlayWidget(this))
{
  // IMPORTANT:
  // Do NOT style the SMeterBarWidget background if you don't want full widget width painted.
  // Style the track instead (it is sized to the bar span).
  setAttribute(Qt::WA_StyledBackground, true);
  setAutoFillBackground(false);
  QWidgetPropertySetter::setWidgetProperty(this, "role", "s-meter-bar", false);

  m_background->setAttribute(Qt::WA_StyledBackground, true);
  m_background->setAutoFillBackground(false);
  QWidgetPropertySetter::setWidgetProperty(m_background, "role", "s-meter-bar-background", false);

  m_background->lower();
  m_colourSegmentOverlay->raise();

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  applyThicknessToSizeHints();
  updateSpanGeometries();
}

void SMeterBarWidget::updateSpanGeometries()
{
  if (!m_background || !m_colourSegmentOverlay) {
    return;
  }

  const QRect span = computeBarSpanRect(this);

  m_background->setGeometry(span);
  m_colourSegmentOverlay->setGeometry(span);

  m_background->show();
  m_colourSegmentOverlay->show();
}

void SMeterBarWidget::resizeEvent(QResizeEvent* e)
{
  QWidget::resizeEvent(e);
  updateSpanGeometries();
}

void SMeterBarWidget::setOrientation(Qt::Orientation o)
{
  if (m_orientation == o) {
    return;
  }
  m_orientation = o;

  if (m_orientation == Qt::Horizontal) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  } else {
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  }

  applyThicknessToSizeHints();
  updateSpanGeometries();
  updateGeometry();
  update();
}

void SMeterBarWidget::setThickness(int px)
{
  m_thicknessPx = std::clamp(px, 8, 60);
  applyThicknessToSizeHints();
  updateSpanGeometries();
  updateGeometry();
  update();
}

void SMeterBarWidget::setEndPadding(int px)
{
  m_endPaddingPx = std::max(0, px);
  updateSpanGeometries();
  update();
}

void SMeterBarWidget::applyThicknessToSizeHints()
{
  if (m_orientation == Qt::Horizontal) {
    setMinimumHeight(m_thicknessPx);
    setMaximumHeight(m_thicknessPx);
    setMinimumWidth(0);
    setMaximumWidth(QWIDGETSIZE_MAX);
  } else {
    setMinimumWidth(m_thicknessPx);
    setMaximumWidth(m_thicknessPx);
    setMinimumHeight(0);
    setMaximumHeight(QWIDGETSIZE_MAX);
  }
}

void SMeterBarWidget::setRange(float minDbFs, float maxDbFs)
{
  if (maxDbFs <= minDbFs) {
    maxDbFs = minDbFs + 1.0f;
  }
  m_minDbFs = minDbFs;
  m_maxDbFs = maxDbFs;
  m_colourSegmentOverlay->update();
}

void SMeterBarWidget::setThresholds(float yellowFromDbFs, float redFromDbFs)
{
  m_yellowFromDbFs = yellowFromDbFs;
  m_redFromDbFs = redFromDbFs;
  if (m_redFromDbFs < m_yellowFromDbFs) {
    std::swap(m_redFromDbFs, m_yellowFromDbFs);
  }
  m_colourSegmentOverlay->update();
}

void SMeterBarWidget::setValue(float valueDbFs)
{
  m_valueDbFs = valueDbFs;
  m_colourSegmentOverlay->update();
}

void SMeterBarWidget::paintEvent(QPaintEvent* /*e*/)
{
  // QPainter p(this);
  // p.setRenderHint(QPainter::Antialiasing, true);
  //
  // QRectF r = rect().adjusted(1, 1, -1, -1);
  //
  // // Inner bounds == bar span
  // QRectF inner = r.adjusted(2, 2, -2, -2);
  // if (m_orientation == Qt::Horizontal) {
  //   inner.adjust(static_cast<qreal>(m_endPaddingPx), 0.0, -static_cast<qreal>(m_endPaddingPx), 0.0);
  // } else {
  //   inner.adjust(0.0, static_cast<qreal>(m_endPaddingPx), 0.0, -static_cast<qreal>(m_endPaddingPx));
  // }
  // if (inner.width() <= 0.0 || inner.height() <= 0.0) {
  //   return;
  // }
  //
  // // Paint QSS background/border ONLY inside the bar span
  // {
  //   QStyleOption opt;
  //   opt.initFrom(this);
  //
  //   p.save();
  //   p.setClipRect(inner);
  //   style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
  //   p.restore();
  // }
  //
  // const float span = std::max(1e-6f, m_maxDbFs - m_minDbFs);
  //
  // auto tAtDb = [&](float db) -> float {
  //   return std::clamp((db - m_minDbFs) / span, 0.0f, 1.0f);
  // };
  //
  // const float vDb = std::clamp(m_valueDbFs, m_minDbFs, m_maxDbFs);
  //
  // const float tValue = tAtDb(vDb);
  // const float tYellow = tAtDb(m_yellowFromDbFs);
  // const float tRed = tAtDb(m_redFromDbFs);
  //
  // auto drawSegment = [&](float t0, float t1, const QColor& c) {
  //   if (t1 <= t0) return;
  //
  //   p.setPen(Qt::NoPen);
  //   p.setBrush(QBrush(c));
  //
  //   if (m_orientation == Qt::Horizontal) {
  //     const qreal x0 = inner.left() + static_cast<qreal>(t0) * inner.width();
  //     const qreal x1 = inner.left() + static_cast<qreal>(t1) * inner.width();
  //
  //     QRectF seg(inner.left(), inner.top(), 0, inner.height());
  //     seg.setLeft(std::max(inner.left(), x0));
  //     seg.setRight(std::min(inner.right(), x1));
  //     if (seg.width() > 0.0) {
  //       p.drawRect(seg);
  //     }
  //   } else {
  //     const qreal y0 = inner.bottom() - static_cast<qreal>(t0) * inner.height();
  //     const qreal y1 = inner.bottom() - static_cast<qreal>(t1) * inner.height();
  //
  //     QRectF seg(inner.left(), inner.top(), inner.width(), 0);
  //     seg.setTop(std::max(inner.top(), y1));
  //     seg.setBottom(std::min(inner.bottom(), y0));
  //     if (seg.height() > 0.0) {
  //       p.drawRect(seg);
  //     }
  //   }
  // };
  //
  // // Fixed segment colors
  // const QColor green(0, 180, 0);
  // const QColor yellow(220, 180, 0);
  // const QColor red(200, 0, 0);
  //
  // const float tMaxFilled = tValue;
  //
  // drawSegment(0.0f, std::min(tYellow, tMaxFilled), green);
  // drawSegment(std::max(tYellow, 0.0f), std::min(tRed, tMaxFilled), yellow);
  // drawSegment(std::max(tRed, 0.0f), tMaxFilled, red);
}

QRect SMeterBarWidget::computeBarSpanRect(const SMeterBarWidget* w)
{
  QRectF r = w->rect().adjusted(1, 1, -1, -1);
  QRectF inner = r.adjusted(2, 2, -2, -2);

  if (w->m_orientation == Qt::Horizontal) {
    inner.adjust(static_cast<qreal>(w->m_endPaddingPx), 0.0, -static_cast<qreal>(w->m_endPaddingPx), 0.0);
  } else {
    inner.adjust(0.0, static_cast<qreal>(w->m_endPaddingPx), 0.0, -static_cast<qreal>(w->m_endPaddingPx));
  }

  return inner.toAlignedRect();
}