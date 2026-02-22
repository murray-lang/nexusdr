//
// Created by murray on 19/2/26.
//
#pragma once

#include "../ScaleRenderer/ScaleRenderer.h"

class SMeterTickProvider final : public IScaleTickProvider
{
public:
  explicit SMeterTickProvider(double s9DbFs = -60.0)
    : m_s9DbFs(s9DbFs)
  {}

  void setS9DbFs(double s9DbFs) { m_s9DbFs = s9DbFs; }
  double s9DbFs() const noexcept { return m_s9DbFs; }

  void setMinorsPerInterval(int n) { m_minorsPerInterval = std::clamp(n, 0, 10); }
  int minorsPerInterval() const noexcept { return m_minorsPerInterval; }

  void setOverS9MaxPlusDb(int maxPlusDb) { m_maxPlusDb = std::max(0, maxPlusDb); }
  int overS9MaxPlusDb() const noexcept { return m_maxPlusDb; }

  void buildTicks(const ScaleModel& model, QVector<ScaleTick>& outTicks) const override;

private:
  double m_s9DbFs = -60.0;
  int m_minorsPerInterval = 1;
  int m_maxPlusDb = 60;
};