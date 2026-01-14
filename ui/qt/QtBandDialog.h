//
// Created by murray on 30/12/25.
//

#pragma once
#include <QDialog>

#include "settings/BandCategory.h"

class Bands;

namespace Ui
{
  class QtBandDialog;
}

class Radio;

class QtBandDialog : public QWidget {
  Q_OBJECT
public:
  explicit QtBandDialog(Radio* pRadio, QWidget *parent = nullptr);
  ~QtBandDialog() override;

  void addCategoryTabs(Radio* pRadio);
  void addCategoryTab(const BandCategory& category, bool isSelected, const std::string& selectedBandName);

protected:
  bool event(QEvent *event) override;
  // void showEvent(QShowEvent *event) override;
  // void mousePressEvent(QMouseEvent *event) override;

private:
  Ui::QtBandDialog *ui;
  Radio* m_pRadio;

  QList<QPushButton*> m_bandButtons;
};