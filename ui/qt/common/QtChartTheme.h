//
// Created by murray on 29/12/25.
//

#pragma once
#include <qstring.h>
#include <QWidget>

class QtChartTheme : public QWidget {
  Q_OBJECT
  Q_PROPERTY(QString plotAreaColor READ plotAreaColor WRITE setPlotAreaColor)
  Q_PROPERTY(QString textColor READ textColor WRITE setTextColor)
  Q_PROPERTY(QString backgroundColor READ backgroundColor WRITE setBackgroundColor)
  Q_PROPERTY(QString seriesLineColor READ seriesLineColor WRITE setSeriesLineColor)
  Q_PROPERTY(QString seriesAreaColor READ seriesAreaColor WRITE setSeriesAreaColor)
  Q_PROPERTY(QString gridColor READ gridColor WRITE setGridColor)
  Q_PROPERTY(QString cursorALineColor READ cursorALineColor WRITE setCursorALineColor)
  Q_PROPERTY(QString cursorAAreaColor READ cursorAAreaColor WRITE setCursorAAreaColor)
  Q_PROPERTY(QString cursorBLineColor READ cursorBLineColor WRITE setCursorBLineColor)
  Q_PROPERTY(QString cursorBAreaColor READ cursorBAreaColor WRITE setCursorBAreaColor)

public:
  explicit QtChartTheme(QWidget* parent = nullptr) : QWidget(parent) {}

  [[nodiscard]] QString plotAreaColor() const { return m_plotAreaColor; }
  void setPlotAreaColor(const QString& c) { m_plotAreaColor = c; }

  [[nodiscard]] QString textColor() const { return m_textColor; }
  void setTextColor(const QString& c) { m_textColor = c; }

  [[nodiscard]] QString backgroundColor() const { return m_backgroundColor; }
  void setBackgroundColor(const QString& c) { m_backgroundColor = c; }

  [[nodiscard]] QString seriesLineColor() const { return m_seriesLineColor; }
  void setSeriesLineColor(const QString& c) { m_seriesLineColor = c; }

  [[nodiscard]] QString seriesAreaColor() const { return m_seriesAreaColor; }
  void setSeriesAreaColor(const QString& c) { m_seriesAreaColor = c; }

  [[nodiscard]] QString gridColor() const { return m_gridColor; }
  void setGridColor(const QString& c) { m_gridColor = c; }

  [[nodiscard]] QString cursorALineColor() const { return m_cursorALineColor; }
  void setCursorALineColor(const QString& c) { m_cursorALineColor = c; }

  [[nodiscard]] QString cursorAAreaColor() const { return m_cursorAAreaColor; }
  void setCursorAAreaColor(const QString& c) { m_cursorAAreaColor = c; }

  [[nodiscard]] QString cursorBLineColor() const { return m_cursorBLineColor; }
  void setCursorBLineColor(const QString& c) { m_cursorBLineColor = c; }

  [[nodiscard]] QString cursorBAreaColor() const { return m_cursorBAreaColor; }
  void setCursorBAreaColor(const QString& c) { m_cursorBAreaColor = c; }

private:
  QString m_textColor;
  QString m_plotAreaColor;
  QString m_backgroundColor;
  QString m_seriesLineColor;
  QString m_seriesAreaColor;
  QString m_gridColor;
  QString m_cursorALineColor;
  QString m_cursorAAreaColor;
  QString m_cursorBLineColor;
  QString m_cursorBAreaColor;
};
