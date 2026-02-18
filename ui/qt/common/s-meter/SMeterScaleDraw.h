//
// Created by murray on 18/2/26.
//

#pragma once

#include <qwt_scale_draw.h>

class SMeterScaleDraw : public QwtScaleDraw
{
public:
  enum class Mode { DbFs, SUnits };

  SMeterScaleDraw();

  void setModeDbFs();
  void setModeSUnits(float s9DbFs);

  QwtText label(double value) const override;

private:
  Mode m_mode;
  float m_s9DbFs;
};