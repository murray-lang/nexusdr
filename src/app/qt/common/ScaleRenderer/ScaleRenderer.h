//
// Created by murray on 19/2/26.
//
#pragma once

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QRect>
#include <QString>
#include <QVector>
#include <Qt>

#include <algorithm>
#include <cmath>
#include <limits>

struct ScaleTick
{
  enum class Kind { Major, Minor };

  double value = 0.0;
  Kind kind = Kind::Major;

  bool hasLabel = false;
  QString label;

  int priority = 0;
};

struct ScaleModel
{
  double min = 0.0;
  double max = 1.0;

  Qt::Orientation orientation = Qt::Horizontal;

  int axisLengthPx = 0;
};

struct ScaleStyle
{
  QColor tickColor = QColor(200, 200, 200);
  QColor labelColor = QColor(200, 200, 200);

  int majorTickPx = 9;
  int minorTickPx = 3;

  int tickToLabelGapPx = 4;
  int textPaddingPx = 2;

  bool hideLabelsWhenTight = true;

  enum class PlacementPolicy { BestFit, Alternate, PreferNearSide, PreferFarSide };
  PlacementPolicy placement = PlacementPolicy::BestFit;

  enum class TextOrientation { HorizontalAlways /* future: AlongAxis */ };
  TextOrientation textOrientation = TextOrientation::HorizontalAlways;

  enum class TickDirection { TowardNearSide, TowardFarSide, Both };
  TickDirection tickDirection = TickDirection::TowardNearSide;
};

struct ScaleLayout
{
  QRect scaleRect;
  QRect axisSpanRect;

  int axisPosPx = 0;

  int nearSideExtentPx = 0;
  int farSideExtentPx = 0;

  int endPaddingPx = 0;
};

class IScaleTickProvider
{
public:
  virtual ~IScaleTickProvider() = default;
  virtual void buildTicks(const ScaleModel& model, QVector<ScaleTick>& outTicks) const = 0;
};

class ScaleRenderer final
{
public:
  void paint(QPainter& p,
             const QFont& font,
             const ScaleStyle& style,
             const ScaleModel& model,
             const ScaleLayout& layout,
             const IScaleTickProvider& tickProvider) const;

  int idealThicknessPx(const QFontMetrics& fm,
                       const ScaleStyle& style,
                       bool allowLabels) const;

private:
  static double safeSpan(double a, double b);
  static double clamp01(double t);

  static double valueToT(double value, const ScaleModel& model);
  static int tToPixel(double t, const ScaleModel& model, const QRect& axisSpanRect);

  static bool rectsOverlapAlongAxis(const QRect& a, const QRect& b, Qt::Orientation o, int gapPx);

  QRect labelRectForTick(const QFontMetrics& fm,
                         const ScaleStyle& style,
                         const ScaleModel& model,
                         const ScaleLayout& layout,
                         int axisPixel,
                         const QString& text,
                         bool farSide) const;

  void drawTick(QPainter& p,
                const ScaleStyle& style,
                const ScaleModel& model,
                const ScaleLayout& layout,
                int axisPixel,
                bool major) const;

  void drawLabel(QPainter& p,
                 const QFont& font,
                 const ScaleStyle& style,
                 const QRect& r,
                 const QString& text) const;
};