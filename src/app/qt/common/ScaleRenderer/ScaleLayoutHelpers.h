//
// Created by murray on 19/2/26.
//
#pragma once

#include "ScaleRenderer.h"

#include <algorithm>

namespace ScaleLayoutHelpers
{
  // Creates a default layout:
  // - axis line centered inside scaleRect
  // - near/far extents are the available pixels to each side of the axis line
  // - endPaddingPx stored but not applied (axisSpanRect should already include it)
  inline ScaleLayout makeCenteredLayout(const QRect& scaleRect,
                                       const QRect& axisSpanRect,
                                       Qt::Orientation orientation,
                                       int endPaddingPx = 0)
  {
    ScaleLayout l;
    l.scaleRect = scaleRect;
    l.axisSpanRect = axisSpanRect;
    l.endPaddingPx = std::max(0, endPaddingPx);

    if (orientation == Qt::Horizontal) {
      // Axis line is horizontal; place it at vertical center of scale band
      l.axisPosPx = scaleRect.center().y();

      // near = below, far = above
      l.nearSideExtentPx = std::max(0, scaleRect.bottom() - l.axisPosPx);
      l.farSideExtentPx  = std::max(0, l.axisPosPx - scaleRect.top());
    } else {
      // Axis line is vertical; place it at horizontal center of scale band
      l.axisPosPx = scaleRect.center().x();

      // near = left, far = right
      l.nearSideExtentPx = std::max(0, l.axisPosPx - scaleRect.left());
      l.farSideExtentPx  = std::max(0, scaleRect.right() - l.axisPosPx);
    }

    return l;
  }

  // Variant: place axis line at a ratio across the band in the perpendicular direction.
  // ratio = 0   => top (horizontal) / left (vertical)
  // ratio = 0.5 => centered
  // ratio = 1   => bottom (horizontal) / right (vertical)
  inline ScaleLayout makeLayoutWithAxisRatio(const QRect& scaleRect,
                                            const QRect& axisSpanRect,
                                            Qt::Orientation orientation,
                                            double ratio01,
                                            int endPaddingPx = 0)
  {
    ratio01 = std::clamp(ratio01, 0.0, 1.0);

    ScaleLayout l;
    l.scaleRect = scaleRect;
    l.axisSpanRect = axisSpanRect;
    l.endPaddingPx = std::max(0, endPaddingPx);

    if (orientation == Qt::Horizontal) {
      const int y = static_cast<int>(std::lround(
        static_cast<double>(scaleRect.top()) +
        (static_cast<double>(scaleRect.height() - 1) * ratio01)
      ));
      l.axisPosPx = y;

      l.nearSideExtentPx = std::max(0, scaleRect.bottom() - l.axisPosPx); // below
      l.farSideExtentPx  = std::max(0, l.axisPosPx - scaleRect.top());    // above
    } else {
      const int x = static_cast<int>(std::lround(
        static_cast<double>(scaleRect.left()) +
        (static_cast<double>(scaleRect.width() - 1) * ratio01)
      ));
      l.axisPosPx = x;

      l.nearSideExtentPx = std::max(0, l.axisPosPx - scaleRect.left());   // left
      l.farSideExtentPx  = std::max(0, scaleRect.right() - l.axisPosPx);  // right
    }

    return l;
  }
}