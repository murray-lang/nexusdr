//
// Created by murray on 19/2/26.
//
#include "LinearTickProvider.h"

#include <algorithm>
#include <cmath>

static constexpr double kEps = 1e-9;

static bool isFinite(double v)
{
  return std::isfinite(v);
}

int LinearTickProvider::autoDecimalsForStep(double step)
{
  if (!isFinite(step) || step <= 0.0) {
    return 0;
  }

  double s = std::abs(step);
  int d = 0;
  while (s < 1.0 && d < 6) {
    s *= 10.0;
    ++d;
  }
  return d;
}

QString LinearTickProvider::formatValue(double v) const
{
  const int dec = (m_decimals >= 0) ? m_decimals : autoDecimalsForStep(m_majorStep);
  return m_prefix + QString::number(v, 'f', dec) + m_suffix;
}

void LinearTickProvider::buildTicks(const ScaleModel& model, QVector<ScaleTick>& outTicks) const
{
  outTicks.clear();

  if (!isFinite(model.min) || !isFinite(model.max)) return;
  if (!isFinite(m_majorStep) || m_majorStep <= 0.0) return;

  const double lo = std::min(model.min, model.max);
  const double hi = std::max(model.min, model.max);
  const double span = hi - lo;

  if (!isFinite(span) || std::abs(span) < 1e-12) {
    ScaleTick t;
    t.value = lo;
    t.kind = ScaleTick::Kind::Major;
    t.hasLabel = (m_labelMode != LabelMode::None);
    if (t.hasLabel) t.label = formatValue(lo);
    t.priority = m_endpointPriority;
    outTicks.push_back(std::move(t));
    return;
  }

  const double step = m_majorStep;
  const double first = std::ceil((lo - kEps) / step) * step;

  QVector<double> majorValues;

  for (double v = first; v <= hi + kEps; v += step) {
    if (v < lo - kEps || v > hi + kEps) continue;

    majorValues.push_back(v);

    ScaleTick t;
    t.value = v;
    t.kind = ScaleTick::Kind::Major;
    t.hasLabel = (m_labelMode == LabelMode::MajorOnly || m_labelMode == LabelMode::All);
    if (t.hasLabel) t.label = formatValue(v);
    t.priority = 10;
    outTicks.push_back(std::move(t));
  }

  auto addEndpointMajor = [&](double v) {
    majorValues.push_back(v);

    ScaleTick t;
    t.value = v;
    t.kind = ScaleTick::Kind::Major;
    t.hasLabel = (m_labelMode == LabelMode::MajorOnly || m_labelMode == LabelMode::All);
    if (t.hasLabel) t.label = formatValue(v);
    t.priority = m_endpointPriority;
    outTicks.push_back(std::move(t));
  };

  if (m_includeEndpoints) {
    addEndpointMajor(lo);
    addEndpointMajor(hi);
  }

  std::sort(majorValues.begin(), majorValues.end());
  majorValues.erase(std::unique(majorValues.begin(), majorValues.end(),
                                [](double a, double b) { return std::abs(a - b) < 1e-12; }),
                    majorValues.end());

  const int minors = std::clamp(m_minorsPerMajor, 0, 50);
  if (minors > 0 && majorValues.size() >= 2) {
    for (int i = 1; i < majorValues.size(); ++i) {
      const double a = majorValues[i - 1];
      const double b = majorValues[i];
      const double interval = b - a;
      if (!isFinite(interval) || std::abs(interval) < 1e-12) continue;

      const double minorStep = interval / static_cast<double>(minors + 1);

      for (int k = 1; k <= minors; ++k) {
        const double v = a + minorStep * static_cast<double>(k);
        if (v < lo - kEps || v > hi + kEps) continue;

        ScaleTick t;
        t.value = v;
        t.kind = ScaleTick::Kind::Minor;

        t.hasLabel = (m_labelMode == LabelMode::All);
        if (t.hasLabel) t.label = formatValue(v);

        t.priority = 0;
        outTicks.push_back(std::move(t));
      }
    }
  }

  std::sort(outTicks.begin(), outTicks.end(), [](const ScaleTick& x, const ScaleTick& y) {
    if (std::abs(x.value - y.value) < 1e-12) {
      return static_cast<int>(x.kind) < static_cast<int>(y.kind);
    }
    return x.value < y.value;
  });
}