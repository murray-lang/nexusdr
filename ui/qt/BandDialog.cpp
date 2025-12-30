//
// Created by murray on 30/12/25.
//

#include "BandDialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QStyle>

#include "ui_BandDialog.h"
#include "settings/Bands.h"
#include "settings/RadioSettings.h"
#include "radio/Radio.h"

BandDialog::BandDialog(Radio* pRadio, QWidget *parent) :
  QDialog(parent, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint), // Set flags here
  ui(new Ui::BandDialog),
  m_pRadio(pRadio)
{
  if (pRadio == nullptr) {
    throw std::runtime_error("BandDialog cannot be created with a null Radio pointer");
  }
  ui->setupUi(this);
  addCategoryTabs(pRadio);
}

BandDialog::~BandDialog() {
  delete ui;
}

bool BandDialog::event(QEvent *event) {
  if (event->type() == QEvent::WindowDeactivate) {
    close();
  }
  return QDialog::event(event);
}

void
BandDialog::addCategoryTabs(Radio* pRadio)
{
  if (pRadio != nullptr) {
    RadioSettings& settings = pRadio->getSettings();
    Band& selectedBand = settings.band;
    const BandCategory* selectedCategory = settings.bands.findCategoryOfBand(selectedBand.getName());

    int selectedTabIndex = 0;
    int currentTabIndex = 0;
    const std::vector<BandCategory>& categories = settings.bands.getCategories();
    for (const auto& category : categories) {
      bool isSelected = selectedCategory != nullptr && category.getName() == selectedCategory->getName();
      if (isSelected) {
        selectedTabIndex = currentTabIndex;
      }
      addCategoryTab(category, isSelected, selectedBand.getName());
      currentTabIndex++;
    }
    ui->tabWidget->setCurrentIndex(selectedTabIndex);
    ui->tabWidget->style()->unpolish(ui->tabWidget);
    ui->tabWidget->style()->polish(ui->tabWidget);
  }
}
void
BandDialog::addCategoryTab(const BandCategory& category, bool isSelected, const std::string& selectedBandName)
{
  QWidget* tab = new QWidget();
  tab->setProperty("class", "toolbarDialogTab");

  QGridLayout* layout = new QGridLayout(tab);
  layout->setContentsMargins(9, 9, 9, 9);
  layout->setSpacing(1);

  const int columns = 5;
  int row = 0;
  int col = 0;

  for (const auto& band : category.getBands()) {
    QPushButton* bandBtn = new QPushButton(QString::fromStdString(band.getLabel()));
    m_bandButtons.append(bandBtn);
    bandBtn->setMaximumHeight(20);
    bandBtn->setMaximumWidth(40);
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
      m_pRadio->applyBand(band.getName());
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