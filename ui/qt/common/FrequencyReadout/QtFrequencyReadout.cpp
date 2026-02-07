//
// Created by murray on 28/1/26.
//

#include "QtFrequencyReadout.h"
#include "ui_QtFrequencyReadout.h"

#include "QtBandReadout.h"
#include "QtVfoReadout.h"

#include <QStyle>

#include "settings/RadioSettings.h"
#include "settings/bands/BandSelector.h"

QtFrequencyReadout::QtFrequencyReadout(QWidget* parent)
  : QWidget(parent)
  , ui(std::make_unique<Ui::QtFrequencyReadout>())
  , m_pSettingsSink(nullptr)
  , m_band1Readout(nullptr)
  , m_band2Readout(nullptr)
{
  ui->setupUi(this);
  setAttribute(Qt::WA_StyledBackground, true);
  initialiseLayout();
}

QtFrequencyReadout::~QtFrequencyReadout() = default;

void
QtFrequencyReadout::initialiseLayout()
{
  // Keep using the .ui gridLayout as a simple 2-row host.
  ui->gridLayout->setContentsMargins(0, 0, 0, 0);
  ui->gridLayout->setHorizontalSpacing(6);
  ui->gridLayout->setVerticalSpacing(2);
  ui->gridLayout->setColumnStretch(0, 1);
  ui->gridLayout->setColumnStretch(1, 1);

  m_band1Readout = new QtBandReadout(SplitBandId::One, this);
  // m_band1Readout->setObjectName("band1Row");
  ui->gridLayout->addWidget(m_band1Readout, 0, 0, 1, 2);

  m_band2Readout = new QtBandReadout(SplitBandId::Two, this);
  // m_band2Readout->setObjectName("band2Row");
  ui->gridLayout->addWidget(m_band2Readout, 1, 0, 1, 2);

  // Wire row-level actions to frequency-readout actions (which produce SettingUpdates).
  connect(m_band1Readout, &QtBandReadout::splitRequested, this, &QtFrequencyReadout::onSplitRequested);
  connect(m_band2Readout, &QtBandReadout::splitRequested, this, &QtFrequencyReadout::onSplitRequested);
  connect(m_band1Readout, &QtBandReadout::closeRequested, this, &QtFrequencyReadout::onCloseRequested);
  connect(m_band2Readout, &QtBandReadout::closeRequested, this, &QtFrequencyReadout::onCloseRequested);

  connect(m_band1Readout, &QtBandReadout::multiVfoActionRequested, this, &QtFrequencyReadout::onMultiVfoActionRequested);
  connect(m_band2Readout, &QtBandReadout::multiVfoActionRequested, this, &QtFrequencyReadout::onMultiVfoActionRequested);

  connect(m_band1Readout, &QtBandReadout::vfoTxActionRequested, this, &QtFrequencyReadout::onVfoTxActionRequested);
  connect(m_band2Readout, &QtBandReadout::vfoTxActionRequested, this, &QtFrequencyReadout::onVfoTxActionRequested);

  connect(m_band1Readout, &QtBandReadout::txBandRequested, this, &QtFrequencyReadout::onTxBandClicked);
  connect(m_band2Readout, &QtBandReadout::txBandRequested, this, &QtFrequencyReadout::onTxBandClicked);

  connect(m_band1Readout, &QtBandReadout::bandClicked, this, &QtFrequencyReadout::onBandClicked);
  connect(m_band2Readout, &QtBandReadout::bandClicked, this, &QtFrequencyReadout::onBandClicked);
  connect(m_band1Readout, &QtBandReadout::vfoClicked, this, &QtFrequencyReadout::onVfoClicked);
  connect(m_band2Readout, &QtBandReadout::vfoClicked, this, &QtFrequencyReadout::onVfoClicked);

  m_band1Readout->setWidgetProperties(false);
  m_band2Readout->setWidgetProperties(false);
  QWidgetPropertySetter::setWidgetProperty(this, "role", "frequencyReadout", true);
}

void
QtFrequencyReadout::initialise(RadioSettings* pRadioSettings)
{
  applyRadioSettings(pRadioSettings, false);
}

void
QtFrequencyReadout::applyRadioSettings(RadioSettings* pRadioSettings, bool onlyIfChanged)
{
  if (pRadioSettings == nullptr) return;

  if (pRadioSettings->hasSettingChanged(RadioSettings::BAND)) {
    BandSelector& bandSelector = pRadioSettings->getBandSelector();
    if (bandSelector.isChanged()) {
      applyBandSelectorChange(bandSelector);
      applyFrequencyChanges(bandSelector, onlyIfChanged);
    }
  } else if (pRadioSettings->hasSettingChanged(RadioSettings::PTT)) {
    setPttProperty(pRadioSettings->getPtt());
  }
}

void
QtFrequencyReadout::applyFrequencyChanges(BandSelector& bandSelector, bool onlyIfChanged)
{
  // Band 1
  if (bandSelector.hasBand(SplitBandId::One)) {
    const BandSettings* band1 = bandSelector.getBandSettings(SplitBandId::One);
    if (band1 != nullptr) {
      m_band1Readout->applyFrequencyChanges(band1, onlyIfChanged);
    }
  }

  // Band 2 (may be absent when not split)
  if (bandSelector.hasBand(SplitBandId::Two)) {
    const BandSettings* band2 = bandSelector.getBandSettings(SplitBandId::Two);
    if (band2 != nullptr) {
      m_band2Readout->applyFrequencyChanges(band2, onlyIfChanged);
    }
  }
}

void
QtFrequencyReadout::applyBandSelectorChange(BandSelector& bandSelector)
{
  const std::string& txBandName = bandSelector.getTxBandName();
  const std::string& rxBandName = bandSelector.getRxBandName();
  const std::string& focusBandName = bandSelector.getFocusBandName();

  const bool hasBand1 = bandSelector.hasBand(SplitBandId::One);
  const bool hasBand2 = bandSelector.hasBand(SplitBandId::Two);
  const bool isSplit = bandSelector.isSplit();

  // Visibility (Band 2 row disappears when not split)
  m_band1Readout->setRowVisible(hasBand1);
  m_band2Readout->setRowVisible(hasBand2);

  if (hasBand1) {
    const BandSettings* band1 = bandSelector.getBandSettings(SplitBandId::One);
    m_band1Readout->applyBandSettings(band1, txBandName, rxBandName, focusBandName);
  }

  if (hasBand2) {
    const BandSettings* band2 = bandSelector.getBandSettings(SplitBandId::Two);
    m_band2Readout->applyBandSettings(band2, txBandName, rxBandName, focusBandName);
  }

  updateRowActionModes(hasBand1, hasBand2, isSplit);

  ui->gridLayout->setRowStretch(0, hasBand1 ? 1 : 0);
  ui->gridLayout->setRowStretch(1, hasBand2 ? 1 : 0);
  ui->gridLayout->invalidate();
  updateGeometry();
}

void
QtFrequencyReadout::updateRowActionModes(bool hasBand1, bool hasBand2, bool isSplit)
{
  // With the row widget, the gutters are fixed and buttons are icon-only,
  // so there is no horizontal jostling; we just change the action mode.

  if (!m_band1Readout || !m_band2Readout) return;

  if (isSplit && hasBand1 && hasBand2) {
    m_band1Readout->setLeftActionMode(QtBandReadout::BandAction::Close);
    m_band2Readout->setLeftActionMode(QtBandReadout::BandAction::Close);
    return;
  }

  // Not split: Band 1 is the only active row => it offers Split.
  if (hasBand1 && !hasBand2) {
    m_band1Readout->setLeftActionMode(QtBandReadout::BandAction::Split);
    m_band2Readout->setLeftActionMode(QtBandReadout::BandAction::Disabled);
    return;
  }

  // Fallback: disable actions (should be rare)
  m_band1Readout->setLeftActionMode(QtBandReadout::BandAction::Disabled);
  m_band2Readout->setLeftActionMode(QtBandReadout::BandAction::Disabled);
}

void
QtFrequencyReadout::onSplitRequested(SplitBandId /*whichBand*/)
{
  if (m_pSettingsSink == nullptr) return;

  SettingUpdatePath splitPath({RadioSettings::BAND, BandSelector::SPLIT});
  SettingUpdate splitSetting(splitPath, true, SettingUpdate::Meaning::VALUE);
  m_pSettingsSink->applySettingUpdate(splitSetting);
}

void
QtFrequencyReadout::onCloseRequested(SplitBandId whichBand)
{
  if (m_pSettingsSink == nullptr) return;

  BandSelector::Features select =
    whichBand == SplitBandId::One ? BandSelector::SELECT_1 : BandSelector::SELECT_2;

  SettingUpdatePath bandPath({RadioSettings::BAND, static_cast<uint32_t>(select)});
  SettingUpdate bandSetting(bandPath, "", SettingUpdate::Meaning::VALUE); // Empty band name closes it
  m_pSettingsSink->applySettingUpdate(bandSetting);
}

void
QtFrequencyReadout::onMultiVfoActionRequested(SplitBandId whichBand,
                                        VfoId whichVfo,
                                        QtVfoReadout::MultiVfoAction action)
{
  uint32_t selectBand = whichBand == SplitBandId::One ? BandSelector::WITH_1 : BandSelector::WITH_2;

  switch (action) {
  case QtVfoReadout::MultiVfoAction::Multi:
    {
      SettingUpdatePath bandPath({
        RadioSettings::BAND,
        selectBand,
        BandSettings::MULTI_PIPELINE
      });
      SettingUpdate bandSetting(bandPath, true, SettingUpdate::Meaning::VALUE);
      m_pSettingsSink->applySettingUpdate(bandSetting);
      break;
    }
  case QtVfoReadout::MultiVfoAction::Close:
    {
      SettingUpdatePath bandPath({
        RadioSettings::BAND,
        selectBand,
        BandSettings::CLOSE_PIPELINE
      });
      SettingUpdate bandSetting(bandPath, whichVfo, SettingUpdate::Meaning::VALUE);
      m_pSettingsSink->applySettingUpdate(bandSetting);
      break;
    }
  }
}

void
QtFrequencyReadout::onVfoTxActionRequested(SplitBandId whichBand,
                            VfoId whichVfo,
                            QtVfoReadout::VfoTxAction action)
{
  uint32_t selectBand = whichBand == SplitBandId::One ? BandSelector::WITH_1 : BandSelector::WITH_2;
  if (action == QtVfoReadout::VfoTxAction::Tx) {
    SettingUpdatePath bandPath({
        RadioSettings::BAND,
        selectBand,
        BandSettings::TX_PIPELINE
      });
    SettingUpdate bandSetting(bandPath, whichVfo, SettingUpdate::Meaning::VALUE);
    m_pSettingsSink->applySettingUpdate(bandSetting);
  }

  switch (action) {
  case QtVfoReadout::VfoTxAction::Tx:
    {
      SettingUpdatePath bandPath({
        RadioSettings::BAND,
        selectBand,
        BandSettings::MULTI_PIPELINE
      });
      SettingUpdate bandSetting(bandPath, true, SettingUpdate::Meaning::VALUE);
      m_pSettingsSink->applySettingUpdate(bandSetting);
      break;
    }

  }
}

void
QtFrequencyReadout::setPttProperty(bool ptt, bool repolish)
{
  m_band1Readout->setPttProperty(ptt, false);
  m_band2Readout->setPttProperty(ptt, false);
  QWidgetPropertySetter::setWidgetProperty(this, "ptt", ptt, repolish);
}

void
QtFrequencyReadout::onBandClicked(SplitBandId whichBand)
{
  if (m_pSettingsSink == nullptr) return;

  SettingUpdatePath path({RadioSettings::BAND, BandSelector::FOCUS});
  SettingUpdate u(path, whichBand, SettingUpdate::Meaning::VALUE);
  m_pSettingsSink->applySettingUpdate(u);
}

void
QtFrequencyReadout::onTxBandClicked(SplitBandId whichBand)
{
  if (m_pSettingsSink == nullptr) return;

  SettingUpdatePath path({RadioSettings::BAND, BandSelector::TX_BAND});
  SettingUpdate update(path, whichBand, SettingUpdate::Meaning::VALUE);
  m_pSettingsSink->applySettingUpdate(update);
}

void
QtFrequencyReadout::onVfoClicked(SplitBandId whichBand, VfoId whichVfo)
{
  if (m_pSettingsSink == nullptr) return;

  const uint32_t selectBand = (whichBand == SplitBandId::One) ? BandSelector::WITH_1 : BandSelector::WITH_2;

  SettingUpdatePath path({
    RadioSettings::BAND,
    selectBand,
    BandSettings::FOCUS_PIPELINE
  });

  SettingUpdate update(path, whichVfo, SettingUpdate::Meaning::VALUE);
  m_pSettingsSink->applySettingUpdate(update);
}