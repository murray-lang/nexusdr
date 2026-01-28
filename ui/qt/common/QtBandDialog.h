//
// Created by murray on 30/12/25.
//

#pragma once
#include <QDialog>

#include "../../../settings/bands/BandCategory.h"
#include "settings/RadioSettingsSink.h"

class Bands;

namespace Ui
{
  class QtBandDialog;
}

class Radio;

class QtBandDialog : public QWidget, public RadioSettingsSink {
  Q_OBJECT
public:
  explicit QtBandDialog(Radio* pRadio, QWidget *parent = nullptr);
  ~QtBandDialog() override;

  void addCategoryTabs(Radio* pRadio);
  void addCategoryTab(const BandCategory& category, bool isSelected, const std::string& selectedBandName);


  // void applySettings(const RadioSettings& settings, BandSettings* pBandSettings) override;

  // These two satisfy the inheritance, but are of no use here. Empty implementations
  void applySettings(const RadioSettings& settings) override;
  void applySettingUpdate(SettingUpdate& settingDelta) override {}

protected:
  void updateTabs(const Band& band);
  void updateBandButtons(const Band& band);

private:
  Ui::QtBandDialog *ui;
  Radio* m_pRadio;

  QList<QPushButton*> m_bandButtons;
};