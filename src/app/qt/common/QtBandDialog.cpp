//
// Created by murray on 30/12/25.
//

#include "QtBandDialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QStyle>
#include <QMouseEvent>

#include "ui_QtBandDialog.h"
#include "core/config-settings/settings/bands/Bands.h"
#include "core/config-settings/settings/RadioSettings.h"
#include "core/radio/Radio.h"

QtBandDialog::QtBandDialog(Radio* pRadio, QWidget *parent) :
  QWidget(parent), // Set flags here
  ui(new Ui::QtBandDialog),
  m_pRadio(pRadio)
{
  if (pRadio == nullptr) {
    throw std::runtime_error("QtBandDialog cannot be created with a null Radio pointer");
  }
  ui->setupUi(this);
  // setFocusPolicy(Qt::StrongFocus);
  addCategoryTabs(pRadio);
  ui->tabWidget->style()->polish(ui->tabWidget);
}

QtBandDialog::~QtBandDialog() {
  delete ui;
}


void
QtBandDialog::applySettings(const RadioSettings& settings)
{
  if (settings.hasSettingChanged(RadioSettings::BAND)) {
    const BandSettings* pBandSettings = settings.getFocusBandSettings();
    const Band& band = pBandSettings->getBand();
    updateTabs(band);
    updateBandButtons(band);
  }
}

void
QtBandDialog::updateTabs(const Band& band)
{
  const Bands& bands = m_pRadio->getBands();
  const BandCategory* bandCategory = bands.findCategoryOfBand(band.getName());
  if (bandCategory != nullptr) {
    int32_t categoryIndex = bands.findCategoryIndex(bandCategory->getName());
    ui->tabWidget->setCurrentIndex(categoryIndex);
  }
}

void
QtBandDialog::updateBandButtons(const Band& band)
{
  for (QPushButton* btn : m_bandButtons) {
    bool isSelected = (btn->text().toStdString() == band.getLabel());
    btn->setProperty("selected", isSelected);
    btn->style()->unpolish(btn);
    btn->style()->polish(btn);
  }
}

void
QtBandDialog::addCategoryTabs(Radio* pRadio)
{
  if (pRadio != nullptr) {
    const RadioSettings& settings = pRadio->getRadioSettings();
    const Bands& bands = pRadio->getBands();
    const std::string& selectedBandName = pRadio->getFocusBandName();
    const BandCategory* selectedCategory = bands.findCategoryOfBand(selectedBandName);

    int selectedTabIndex = 0;
    int currentTabIndex = 0;
    const std::vector<BandCategory>& categories = bands.getCategories();
    for (const auto& category : categories) {
      bool isSelected = selectedCategory != nullptr && category.getName() == selectedCategory->getName();
      if (isSelected) {
        selectedTabIndex = currentTabIndex;
      }
      addCategoryTab(category, isSelected, selectedBandName);
      currentTabIndex++;
    }
    ui->tabWidget->setCurrentIndex(selectedTabIndex);
    ui->tabWidget->style()->unpolish(ui->tabWidget);
    ui->tabWidget->style()->polish(ui->tabWidget);
  }
}
void
QtBandDialog::addCategoryTab(const BandCategory& category, bool isSelected, const std::string& selectedBandName)
{
  QWidget* tab = new QWidget();
  tab->setProperty("class", "toolbarDialogTab");

  QGridLayout* layout = new QGridLayout(tab);
  layout->setContentsMargins(9, 9, 9, 9);
  layout->setSpacing(1);
  layout->setVerticalSpacing(10);

  const int columns = 5;
  int row = 0;
  int col = 0;

  for (const auto& band : category.getBands()) {
    QPushButton* bandBtn = new QPushButton(QString::fromStdString(band.getLabel()));
    m_bandButtons.append(bandBtn);
    bandBtn->setMaximumHeight(40);
    bandBtn->setMaximumWidth(70);
    bandBtn->setProperty("class", "toolbarDialogButton");

    bool isSelectedBand = band.getName() == selectedBandName;
    bandBtn->setProperty("selected", isSelectedBand);
    bandBtn->style()->unpolish(bandBtn);
    bandBtn->style()->polish(bandBtn);

    connect(bandBtn, &QPushButton::clicked, this, [this, band, bandBtn]() {
      for (QPushButton* btn : m_bandButtons) {
        if (btn->property("selected").toBool()) {
          btn->setProperty("selected", false);
          btn->style()->unpolish(btn);
          btn->style()->polish(btn);
        }
      }
      bandBtn->setProperty("selected", true);
      bandBtn->style()->unpolish(bandBtn);
      bandBtn->style()->polish(bandBtn);

      m_pRadio->applyBand(band.getName()); // <-- This is what it's all for
      // this->close();
    });

    layout->addWidget(bandBtn, row, col);

    col++;
    if (col >= columns) {
      col = 0;
      row++;
    }
  }

  // Add a spacer to push buttons to the top
  layout->setRowStretch(row + 1, 1);

  ui->tabWidget->addTab(tab, QString::fromStdString(category.getLabel()));
}