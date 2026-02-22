//
// Created by murray on 19/2/26.
//
#pragma once

#include "ScaleRenderer.h"

#include <QString>

class NiceNumberTickProvider final : public IScaleTickProvider
{
public:
  enum class LabelMode
  {
    None,
    MajorOnly,
    All
  };

  NiceNumberTickProvider() = default;

  // Target pixels between major ticks (density control).
  void setTargetMajorPx(int px) { m_targetMajorPx = std::clamp(px, 10, 400); }
  int targetMajorPx() const noexcept { return m_targetMajorPx; }

  // If true, include endpoints (min/max) as major ticks.
  void setIncludeEndpoints(bool on) { m_includeEndpoints = on; }
  bool includeEndpoints() const noexcept { return m_includeEndpoints; }

  // Minor ticks per major interval.
  void setMinorsPerMajor(int n) { m_minorsPerMajor = std::clamp(n, 0, 50); }
  int minorsPerMajor() const noexcept { return m_minorsPerMajor; }

  // Labels
  void setLabelMode(LabelMode m) { m_labelMode = m; }
  LabelMode labelMode() const noexcept { return m_labelMode; }

  // Numeric formatting
  void setDecimals(int decimals) { m_decimals = decimals; }
  int decimals() const noexcept { return m_decimals; }

  void setPrefix(const QString& s) { m_prefix = s; }
  const QString& prefix() const noexcept { return m_prefix; }

  void setSuffix(const QString& s) { m_suffix = s; }
  const QString& suffix() const noexcept { return m_suffix; }

  // Priorities
  void setEndpointPriority(int p) { m_endpointPriority = p; }
  int endpointPriority() const noexcept { return m_endpointPriority; }

  void setMajorPriority(int p) { m_majorPriority = p; }
  int majorPriority() const noexcept { return m_majorPriority; }

  void buildTicks(const ScaleModel& model, QVector<ScaleTick>& outTicks) const override;

private:
  static double niceStep(double rawStep);
  static int autoDecimalsForStep(double step);
  QString formatValue(double v, double chosenMajorStep) const;

  int m_targetMajorPx = 60;
  bool m_includeEndpoints = false;
  int m_minorsPerMajor = 4;

  LabelMode m_labelMode = LabelMode::MajorOnly;
  int m_decimals = -1;
  QString m_prefix;
  QString m_suffix;

  int m_endpointPriority = 50;
  int m_majorPriority = 10;
};