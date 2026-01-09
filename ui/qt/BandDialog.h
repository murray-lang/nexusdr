//
// Created by murray on 30/12/25.
//

#pragma once
#include <QDialog>

#include "settings/BandCategory.h"

class Bands;

namespace Ui
{
  class BandDialog;
}

class Radio;

class BandDialog : public QDialog {
  Q_OBJECT
public:
  explicit BandDialog(Radio* pRadio, QWidget *parent = nullptr);
  ~BandDialog() override;

  void addCategoryTabs(Radio* pRadio);
  void addCategoryTab(const BandCategory& category, bool isSelected, const std::string& selectedBandName);

protected:
  bool event(QEvent *event) override;

private:
  Ui::BandDialog *ui;
  Radio* m_pRadio;

  QList<QPushButton*> m_bandButtons;
};