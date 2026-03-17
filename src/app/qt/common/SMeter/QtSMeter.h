//
// Created by murray on 19/2/26.
//

#pragma once

#include <QColor>
#include <QWidget>
#include <optional>

class QtSMeter final : public QWidget
{
  Q_OBJECT

  // -----------------------
  // Theme-able properties (QSS via qproperty_*)
  // -----------------------
  Q_PROPERTY(int barThicknessPx READ barThickness WRITE setBarThickness)
  Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
  Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
  Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth)

  Q_PROPERTY(QColor barOkColor READ barOkColor WRITE setBarOkColor)
  Q_PROPERTY(QColor barWarnColor READ barWarnColor WRITE setBarWarnColor)
  Q_PROPERTY(QColor barAlarmColor READ barAlarmColor WRITE setBarAlarmColor)
  Q_PROPERTY(QColor barTrackColor READ barTrackColor WRITE setBarTrackColor)

  Q_PROPERTY(QColor tickColor READ tickColor WRITE setTickColor)
  Q_PROPERTY(QColor labelColor READ labelColor WRITE setLabelColor)

  Q_PROPERTY(int majorTickPx READ majorTickPx WRITE setMajorTickPx)
  Q_PROPERTY(int minorTickPx READ minorTickPx WRITE setMinorTickPx)
  Q_PROPERTY(int tickToLabelGapPx READ tickToLabelGapPx WRITE setTickToLabelGapPx)
  Q_PROPERTY(bool hideLabelsWhenTight READ hideLabelsWhenTight WRITE setHideLabelsWhenTight)

  Q_PROPERTY(int endPaddingPx READ endPaddingPx WRITE setEndPaddingPx)
  Q_PROPERTY(int barInsetPx READ barInsetPx WRITE setBarInsetPx)

  Q_PROPERTY(bool showScale READ showScale WRITE setShowScale)

  // Machine-friendly (C++): enum
  Q_PROPERTY(ScalePlacement scalePlacement READ scalePlacement WRITE setScalePlacement)

  // Human-friendly (QSS): "auto" | "top" | "bottom" | "left" | "right"
  // If set to a non-empty value, this overrides scalePlacement.
  Q_PROPERTY(QString scalePlacementText READ scalePlacementText WRITE setScalePlacementText)

  // S-meter range control above S9 (in dB). Typical: 40.
  Q_PROPERTY(int overS9MaxDb READ overS9MaxDb WRITE setOverS9MaxDb)
  Q_PROPERTY(int scalePadPx READ scalePadPx WRITE setScalePadPx)

public:
  enum class ScalePlacement
  {
    Auto = 0,
    Top = 1,
    Bottom = 2,
    Left = 3,
    Right = 4
  };
  Q_ENUM(ScalePlacement)

  explicit QtSMeter(QWidget* parent = nullptr);
  ~QtSMeter() override = default;

  // -----------------------
  // Runtime configuration
  // -----------------------
  void setRangeDbFs(float minDbFs, float maxDbFs);

  void setReferenceDbFs(float referenceDbFs, float overS9MaxDb = 40.0f);
  void setZoneThresholdsDbFs(float yellowFromDbFs, float redFromDbFs);

  void setOrientation(Qt::Orientation o);
  [[nodiscard]] Qt::Orientation orientation() const noexcept { return m_orientation; }

  [[nodiscard]] int barThickness() const noexcept { return m_barThicknessPx; }
  void setBarThickness(int px);

  void setReading(float rssiDbFs, std::optional<float> agcGainDb = std::nullopt);

  // -----------------------
  // Scale visibility + placement
  // -----------------------
  [[nodiscard]] bool showScale() const noexcept { return m_showScale; }
  void setShowScale(bool on);

  [[nodiscard]] ScalePlacement scalePlacement() const noexcept { return m_scalePlacement; }
  void setScalePlacement(ScalePlacement placement);

  [[nodiscard]] QString scalePlacementText() const { return m_scalePlacementText; }
  void setScalePlacementText(const QString& text);

  // -----------------------
  // S-meter-specific range control
  // -----------------------
  [[nodiscard]] int overS9MaxDb() const noexcept { return m_overS9MaxDb; }
  void setOverS9MaxDb(int db);

  // -----------------------
  // Theme properties API
  // -----------------------
  [[nodiscard]] QColor backgroundColor() const noexcept { return m_backgroundColor; }
  void setBackgroundColor(const QColor& c);

  [[nodiscard]] QColor borderColor() const noexcept { return m_borderColor; }
  void setBorderColor(const QColor& c);

  [[nodiscard]] int borderWidth() const noexcept { return m_borderWidth; }
  void setBorderWidth(int px);

  [[nodiscard]] QColor barOkColor() const noexcept { return m_barOkColor; }
  void setBarOkColor(const QColor& c);

  [[nodiscard]] QColor barWarnColor() const noexcept { return m_barWarnColor; }
  void setBarWarnColor(const QColor& c);

  [[nodiscard]] QColor barAlarmColor() const noexcept { return m_barAlarmColor; }
  void setBarAlarmColor(const QColor& c);

  [[nodiscard]] QColor barTrackColor() const noexcept { return m_barTrackColor; }
  void setBarTrackColor(const QColor& c);

  [[nodiscard]] QColor tickColor() const noexcept { return m_tickColor; }
  void setTickColor(const QColor& c);

  [[nodiscard]] QColor labelColor() const noexcept { return m_labelColor; }
  void setLabelColor(const QColor& c);

  [[nodiscard]] int majorTickPx() const noexcept { return m_majorTickPx; }
  void setMajorTickPx(int px);

  [[nodiscard]] int minorTickPx() const noexcept { return m_minorTickPx; }
  void setMinorTickPx(int px);

  [[nodiscard]] int tickToLabelGapPx() const noexcept { return m_tickToLabelGapPx; }
  void setTickToLabelGapPx(int px);

  [[nodiscard]] bool hideLabelsWhenTight() const noexcept { return m_hideLabelsWhenTight; }
  void setHideLabelsWhenTight(bool on);

  [[nodiscard]] int endPaddingPx() const noexcept { return m_endPaddingPx; }
  void setEndPaddingPx(int px);

  [[nodiscard]] int barInsetPx() const noexcept { return m_barInsetPx; }
  void setBarInsetPx(int px);

  [[nodiscard]] int scalePadPx() const noexcept { return m_scalePadPx; }
  void setScalePadPx(int px);

  [[nodiscard]] QSize sizeHint() const override;
  [[nodiscard]] QSize minimumSizeHint() const override;

protected:
  void paintEvent(QPaintEvent* e) override;
  bool event(QEvent* e) override;

private:
  void updateToolTip(float rssiDbFs, const std::optional<float>& agcGainDb);

  [[nodiscard]] int scaleBandThicknessPx() const;

  [[nodiscard]] ScalePlacement effectiveScalePlacement() const;
  [[nodiscard]] ScalePlacement parsePlacementTextOr(ScalePlacement fallback) const;

  [[nodiscard]] double axisRatioForPlacement(ScalePlacement p) const;
  [[nodiscard]] int tickDirectionForPlacement(ScalePlacement p) const;

  [[nodiscard]] QRect barRect(const QRect& outer) const;
  [[nodiscard]] QRect scaleRect(const QRect& outer) const;
  [[nodiscard]] QRect barInnerFillRect(const QRect& barR) const;

  [[nodiscard]] double clamp01(double t) const;
  [[nodiscard]] double valueToT(double valueDbFs) const;
  [[nodiscard]] int tToPixelAlongAxis(double t, const QRect& axisSpanRect) const;

  void paintBackgroundAndBorder(QPainter& p, const QRect& outer) const;
  void paintBar(QPainter& p, const QRect& barR) const;
  void paintScale(QPainter& p,
                  const QRect& outer,
                  const QRect& barR,
                  const QRect& barFillRect,
                  const QRect& barInnerAxisSpan) const;

private:
  Qt::Orientation m_orientation = Qt::Horizontal;

  float m_minDbFs = -114.0f;
  float m_maxDbFs = -20.0f;
  float m_s9DbFs = -60.0f;

  float m_yellowFromDbFs = -50.0f;
  float m_redFromDbFs = -35.0f;

  float m_lastValueDbFs = -120.0f;
  QString m_lastToolTip;

  int m_barThicknessPx = 6;

  bool m_showScale = true;
  ScalePlacement m_scalePlacement = ScalePlacement::Auto;
  QString m_scalePlacementText;

  int m_overS9MaxDb = 40;

  QColor m_backgroundColor = QColor(16, 18, 21);
  QColor m_borderColor = QColor(48, 52, 58);
  int m_borderWidth = 1;

  QColor m_barOkColor = QColor(59, 209, 111);
  QColor m_barWarnColor = QColor(255, 210, 74);
  QColor m_barAlarmColor = QColor(255, 77, 77);
  QColor m_barTrackColor = QColor(30, 33, 38);

  QColor m_tickColor = QColor(200, 205, 211);
  QColor m_labelColor = QColor(200, 205, 211);

  int m_majorTickPx = 5;
  int m_minorTickPx = 2;
  int m_tickToLabelGapPx = 2;
  bool m_hideLabelsWhenTight = true;

  int m_endPaddingPx = 12;
  int m_barInsetPx = 1;
  int m_scalePadPx = 4;
};