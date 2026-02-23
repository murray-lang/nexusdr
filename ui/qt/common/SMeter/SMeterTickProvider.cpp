//
// Created by murray on 19/2/26.
//
#include "SMeterTickProvider.h"

#include <algorithm>

static constexpr double kEps = 1e-9;

void SMeterTickProvider::buildTicks(const ScaleModel& model, QVector<ScaleTick>& outTicks) const
{
  outTicks.clear();

  const double lo = std::min(model.min, model.max);
  const double hi = std::max(model.min, model.max);

  auto inRange = [&](double v) {
    return (v >= lo - kEps) && (v <= hi + kEps);
  };

  QVector<double> majors;

  auto addMajor = [&](double v, const QString& label, int priority, bool hasLabel = true) {
    if (!inRange(v)) return;
    majors.push_back(v);

    ScaleTick t;
    t.value = v;
    t.kind = ScaleTick::Kind::Major;
    t.hasLabel = hasLabel;
    t.label = hasLabel ? label : QString{};
    t.priority = priority;
    outTicks.push_back(std::move(t));
  };

  // -----------------------
  // Below S9: include S0..S9 (6 dB steps)
  // -----------------------
  // -----------------------
  // Below S9: include S0..S9 (6 dB steps)
  // -----------------------
  for (int s = 0; s <= 9; ++s) {
    const double v = m_s9DbFs + (static_cast<double>(s) - 9.0) * 6.0;
    // Labels as plain numbers: 0..9
    const QString label = QString::number(s);
    // Make 9 “stickiest”
    const int prio = (s == 9) ? 100 : 10;
    addMajor(v, label, prio, /*hasLabel=*/true);
  }

  // -----------------------
  // Above S9: +10, +20, ...
  // -----------------------
  for (int plus = 10; plus <= m_maxPlusDb; plus += 10) {
    const double v = m_s9DbFs + static_cast<double>(plus);
    addMajor(v, QString("+%1").arg(plus), 20, /*hasLabel=*/true);
  }

  std::sort(majors.begin(), majors.end());
  majors.erase(std::unique(majors.begin(), majors.end()), majors.end());

  // Minor ticks between adjacent majors
  if (m_minorsPerInterval > 0 && majors.size() >= 2) {
    for (int i = 1; i < majors.size(); ++i) {
      const double a = majors[i - 1];
      const double b = majors[i];
      const double step = (b - a) / static_cast<double>(m_minorsPerInterval + 1);

      for (int k = 1; k <= m_minorsPerInterval; ++k) {
        const double v = a + step * static_cast<double>(k);
        if (!inRange(v)) continue;

        ScaleTick t;
        t.value = v;
        t.kind = ScaleTick::Kind::Minor;
        t.hasLabel = false;
        t.priority = 0;

        outTicks.push_back(std::move(t));
      }
    }
  }

  std::sort(outTicks.begin(), outTicks.end(), [](const ScaleTick& x, const ScaleTick& y) {
    if (x.value == y.value) return static_cast<int>(x.kind) < static_cast<int>(y.kind);
    return x.value < y.value;
  });
}