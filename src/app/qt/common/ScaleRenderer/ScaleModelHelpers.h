//
// Created by murray on 19/2/26.
//
#pragma once

#include "ScaleRenderer.h"

#include <algorithm>

namespace ScaleModelHelpers
{
  inline int axisLengthPxFromLayout(Qt::Orientation orientation, const ScaleLayout& layout)
  {
    const int len = (orientation == Qt::Horizontal)
      ? layout.axisSpanRect.width()
      : layout.axisSpanRect.height();

    // Guard against empty/negative rects (Qt can give width/height <= 0 for invalid rects)
    return std::max(0, len);
  }

  inline ScaleModel makeModel(double min,
                              double max,
                              Qt::Orientation orientation,
                              const ScaleLayout& layout)
  {
    ScaleModel m;
    m.min = min;
    m.max = max;
    m.orientation = orientation;
    m.axisLengthPx = axisLengthPxFromLayout(orientation, layout);
    return m;
  }
}