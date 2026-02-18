//
// Created by murray on 18/2/26.
//

#pragma once

#include <QWidget>
#include <optional>

class QwtScaleWidget;
class SMeterBarWidget;

class QtSMeter : public QWidget
{
  Q_OBJECT
public:
  explicit QtSMeter(QWidget* parent = nullptr);

  void setRangeDbFs(float minDbFs, float maxDbFs);
  void setZoneThresholdsDbFs(float yellowFromDbFs, float redFromDbFs);

  void setOrientation(Qt::Orientation o);
  void setThickness(int px);
  [[nodiscard]] int thickness() const;

  void setScaleFontPointSize(int pt);
  void setScaleTicks(float majorStepDb, int minorTicksPerMajor);

  void setScaleLabelsDbFs();
  void setScaleLabelsSUnits(float s9DbFs);
  void setReferenceDbFs(float referenceDbFs, float overS9MaxDb = 40.0f);

  void setReading(float rssiDbFs, std::optional<float> agcGainDb);

private:
  void updateToolTip(float rssiDbFs, const std::optional<float>& agcGainDb);
  void applyScale();

private:
  QwtScaleWidget* m_pScale;
  SMeterBarWidget* m_pBar;

  Qt::Orientation m_orientation;

  float m_minDbFs;
  float m_maxDbFs;
  float m_s9DbFs;

  float m_yellowFromDbFs;
  float m_redFromDbFs;

  float m_lastValueDbFs;
  QString m_lastToolTip;

  float m_majorStepDb;
  int m_minorTicksPerMajor;
};