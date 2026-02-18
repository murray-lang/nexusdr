//
// Created by murray on 18/2/26.
//
#pragma once
#include "SMeterBarWidget.h"

#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QResizeEvent>
#include <algorithm>

class BarOverlayWidget final : public QWidget
{
public:
  explicit BarOverlayWidget(SMeterBarWidget* owner)
    : QWidget(owner)
    , m_owner(owner)
  {
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setAttribute(Qt::WA_StyledBackground, false);
    setAutoFillBackground(false);
  }

protected:
  void paintEvent(QPaintEvent*) override
  {
    if (!m_owner) {
      return;
    }

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const QRectF inner = rect();
    if (inner.width() <= 0.0 || inner.height() <= 0.0) {
      return;
    }

    const float span = std::max(1e-6f, m_owner->m_maxDbFs - m_owner->m_minDbFs);

    auto tAtDb = [&](float db) -> float {
      return std::clamp((db - m_owner->m_minDbFs) / span, 0.0f, 1.0f);
    };

    const float vDb = std::clamp(m_owner->m_valueDbFs, m_owner->m_minDbFs, m_owner->m_maxDbFs);

    const float tValue = tAtDb(vDb);
    const float tYellow = tAtDb(m_owner->m_yellowFromDbFs);
    const float tRed = tAtDb(m_owner->m_redFromDbFs);

    auto drawSegment = [&](float t0, float t1, const QColor& c) {
      if (t1 <= t0) return;

      p.setPen(Qt::NoPen);
      p.setBrush(QBrush(c));

      if (m_owner->m_orientation == Qt::Horizontal) {
        const qreal x0 = inner.left() + static_cast<qreal>(t0) * inner.width();
        const qreal x1 = inner.left() + static_cast<qreal>(t1) * inner.width();

        QRectF seg(inner.left(), inner.top(), 0, inner.height());
        seg.setLeft(std::max(inner.left(), x0));
        seg.setRight(std::min(inner.right(), x1));
        if (seg.width() > 0.0) {
          p.drawRect(seg);
        }
      } else {
        const qreal y0 = inner.bottom() - static_cast<qreal>(t0) * inner.height();
        const qreal y1 = inner.bottom() - static_cast<qreal>(t1) * inner.height();

        QRectF seg(inner.left(), inner.top(), inner.width(), 0);
        seg.setTop(std::max(inner.top(), y1));
        seg.setBottom(std::min(inner.bottom(), y0));
        if (seg.height() > 0.0) {
          p.drawRect(seg);
        }
      }
    };

    // Fixed segment colors
    const QColor green(0, 180, 0);
    const QColor yellow(220, 180, 0);
    const QColor red(200, 0, 0);

    const float tMaxFilled = tValue;

    drawSegment(0.0f, std::min(tYellow, tMaxFilled), green);
    drawSegment(std::max(tYellow, 0.0f), std::min(tRed, tMaxFilled), yellow);
    drawSegment(std::max(tRed, 0.0f), tMaxFilled, red);
  }

private:
  SMeterBarWidget* m_owner;
};
