//
// Created by murray on 19/2/26.
//
#include "NiceNumberTickProvider.h"

#include <algorithm>
#include <cmath>

static constexpr double kEps = 1e-9;

static bool isFinite(double v)
{
  return std::isfinite(v);
}

static double safeSpan(double lo, double hi)
{
  const double s = hi - lo;
  return (std::abs(s) < 1e-12) ? 1.0 : s;
}

double NiceNumberTickProvider::niceStep(double rawStep)
{
  if (!isFinite(rawStep) || rawStep <= 0.0) return 1.0;

  const double exp10 = std::floor(std::log10(rawStep));
  const double base = std::pow(10.0, exp10);
  const double f = rawStep / base; // in [1..10)

  double niceF = 1.0;
  if (f <= 1.0) niceF = 1.0;
  else if (f <= 2.0) niceF = 2.0;
  else if (f <= 5.0) niceF = 5.0;
  else niceF = 10.0;

  return niceF * base;
}

int NiceNumberTickProvider::autoDecimalsForStep(double step)
{
  if (!isFinite(step) || step <= 0.0) return 0;

  double s = std::abs(step);
  int d = 0;
  while (s < 1.0 && d < 8) {
    s *= 10.0;
    ++d;
  }
  return d;
}

QString NiceNumberTickProvider::formatValue(double v, double chosenMajorStep) const
{
  const int dec = (m_decimals >= 0) ? m_decimals : autoDecimalsForStep(chosenMajorStep);
  return m_prefix + QString::number(v, 'f', dec) + m_suffix;
}

void NiceNumberTickProvider::buildTicks(const ScaleModel& model, QVector<ScaleTick>& outTicks) const
{
  outTicks.clear();

  if (!isFinite(model.min) || !isFinite(model.max)) return;

  const double lo = std::min(model.min, model.max);
  const double hi = std::max(model.min, model.max);
  const double span = safeSpan(lo, hi);

  if (!isFinite(span) || std::abs(span) < 1e-12) {
    ScaleTick t;
    t.value = lo;
    t.kind = ScaleTick::Kind::Major;
    t.hasLabel = (m_labelMode != LabelMode::None);
    if (t.hasLabel) t.label = formatValue(lo, 1.0);
    t.priority = m_endpointPriority;
    outTicks.push_back(std::move(t));
    return;
  }

  // Pixel-aware major count choice:
  // if axisLengthPx isn't set, fall back to a reasonable guess.
  const int axisPx = (model.axisLengthPx > 0) ? model.axisLengthPx : 300;
  const int targetPx = std::max(10, m_targetMajorPx);

  int desiredMajors = std::clamp(axisPx / targetPx, 2, 12);

  const double rawStep = span / static_cast<double>(desiredMajors);
  const double majorStep = niceStep(rawStep);

  const double first = std::ceil((lo - kEps) / majorStep) * majorStep;

  QVector<double> majors;

  for (double v = first; v <= hi + kEps; v += majorStep) {
    if (v < lo - kEps || v > hi + kEps) continue;

    majors.push_back(v);

    ScaleTick t;
    t.value = v;
    t.kind = ScaleTick::Kind::Major;
    t.hasLabel = (m_labelMode == LabelMode::MajorOnly || m_labelMode == LabelMode::All);
    if (t.hasLabel) t.label = formatValue(v, majorStep);
    t.priority = m_majorPriority;

    outTicks.push_back(std::move(t));
  }

  auto addEndpointMajor = [&](double v) {
    majors.push_back(v);

    ScaleTick t;
    t.value = v;
    t.kind = ScaleTick::Kind::Major;
    t.hasLabel = (m_labelMode == LabelMode::MajorOnly || m_labelMode == LabelMode::All);
    if (t.hasLabel) t.label = formatValue(v, majorStep);
    t.priority = m_endpointPriority;

    outTicks.push_back(std::move(t));
  };

  if (m_includeEndpoints) {
    addEndpointMajor(lo);
    addEndpointMajor(hi);
  }

  std::sort(majors.begin(), majors.end());
  majors.erase(std::unique(majors.begin(), majors.end(),
                           [](double a, double b) { return std::abs(a - b) < 1e-12; }),
               majors.end());

  const int minors = std::clamp(m_minorsPerMajor, 0, 50);
  if (minors > 0 && majors.size() >= 2) {
    for (int i = 1; i < majors.size(); ++i) {
      const double a = majors[i - 1];
      const double b = majors[i];
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
        if (t.hasLabel) t.label = formatValue(v, majorStep);
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