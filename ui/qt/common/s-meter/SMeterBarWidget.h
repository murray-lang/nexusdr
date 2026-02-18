//
// Created by murray on 18/2/26.
//

#pragma once

#include <QWidget>

class SMeterBarWidget : public QWidget
{
  friend class BarOverlayWidget;
  Q_OBJECT
public:
  explicit SMeterBarWidget(QWidget* parent = nullptr);

  void setThickness(int px);
  [[nodiscard]] int thickness() const { return m_thicknessPx; }
  void setOrientation(Qt::Orientation o);

  void setEndPadding(int px);

  void setRange(float minDbFs, float maxDbFs);
  void setThresholds(float yellowFromDbFs, float redFromDbFs);
  void setValue(float valueDbFs);

protected:
  void paintEvent(QPaintEvent* e) override;
  void resizeEvent(QResizeEvent* e) override;

  void applyThicknessToSizeHints();

  void updateSpanGeometries();

  static QRect computeBarSpanRect(const SMeterBarWidget* w);

private:
  Qt::Orientation m_orientation;
  int m_thicknessPx;
  int m_endPaddingPx;

  float m_minDbFs;
  float m_maxDbFs;
  float m_yellowFromDbFs;
  float m_redFromDbFs;
  float m_valueDbFs;

  QWidget* m_background;            // styled background/border (QSS), sized to bar span
  QWidget* m_colourSegmentOverlay;  // was m_overlay: draws segments on top
};