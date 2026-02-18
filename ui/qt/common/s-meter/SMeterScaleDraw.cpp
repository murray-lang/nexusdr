//
// Created by murray on 18/2/26.
//

#include "SMeterScaleDraw.h"

#include <qwt_text.h>
#include <QString>
#include <QColor>
#include <QFont>
#include <cmath>
#include <algorithm>
#include <qdebug.h>

SMeterScaleDraw::SMeterScaleDraw()
  : m_mode(Mode::SUnits)
  , m_s9DbFs(-60.0f) // placeholder; set via setModeSUnits()
{

}

void SMeterScaleDraw::setModeDbFs()
{
  m_mode = Mode::DbFs;
}

void SMeterScaleDraw::setModeSUnits(float s9DbFs)
{
  m_mode = Mode::SUnits;
  m_s9DbFs = s9DbFs;
}

QwtText
SMeterScaleDraw::label(double value) const
{
  QwtText t;

  if (m_mode == Mode::DbFs) {
    const int v = static_cast<int>(std::lround(value));
    t = QwtText(QString::number(v));
  } else {
    const double d = value - static_cast<double>(m_s9DbFs);

    if (d >= 0.0) {
      const int plus10 = static_cast<int>(std::lround(d / 10.0)) * 10;
      t = (plus10 == 0) ? QwtText("9") : QwtText(QString("+%1").arg(plus10));
    } else {
      const int s = static_cast<int>(std::lround(9.0 + d / 6.0));
      const int sClamped = std::clamp(s, 1, 9);
      t = QwtText(QString("%1").arg(sClamped));
    }
  }

  // Force readable label color (helps when palette/theme makes them invisible)
  // QFont f;
  // f.setPointSize(8);
  // t.setFont(f);
  // t.setColor(QColor(220, 220, 220));
  return t;
}