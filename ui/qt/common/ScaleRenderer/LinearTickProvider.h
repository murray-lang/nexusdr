//
// Created by murray on 19/2/26.
//
#pragma once

#include "ScaleRenderer.h"

#include <QString>

class LinearTickProvider final : public IScaleTickProvider
{
public:
  enum class LabelMode
  {
    None,
    MajorOnly,
    All
  };

  LinearTickProvider() = default;

  void setMajorStep(double step) { m_majorStep = (step > 0.0) ? step : 1.0; }
  double majorStep() const noexcept { return m_majorStep; }

  void setMinorsPerMajor(int n) { m_minorsPerMajor = std::clamp(n, 0, 50); }
  int minorsPerMajor() const noexcept { return m_minorsPerMajor; }

  void setIncludeEndpoints(bool on) { m_includeEndpoints = on; }
  bool includeEndpoints() const noexcept { return m_includeEndpoints; }

  void setLabelMode(LabelMode m) { m_labelMode = m; }
  LabelMode labelMode() const noexcept { return m_labelMode; }

  void setDecimals(int decimals) { m_decimals = decimals; }
  int decimals() const noexcept { return m_decimals; }

  void setPrefix(const QString& s) { m_prefix = s; }
  const QString& prefix() const noexcept { return m_prefix; }

  void setSuffix(const QString& s) { m_suffix = s; }
  const QString& suffix() const noexcept { return m_suffix; }

  void setEndpointPriority(int p) { m_endpointPriority = p; }
  int endpointPriority() const noexcept { return m_endpointPriority; }

  void buildTicks(const ScaleModel& model, QVector<ScaleTick>& outTicks) const override;

private:
  static int autoDecimalsForStep(double step);
  QString formatValue(double v) const;

  double m_majorStep = 10.0;
  int m_minorsPerMajor = 4;
  bool m_includeEndpoints = false;

  LabelMode m_labelMode = LabelMode::MajorOnly;
  int m_decimals = -1;

  QString m_prefix;
  QString m_suffix;

  int m_endpointPriority = 50;
};