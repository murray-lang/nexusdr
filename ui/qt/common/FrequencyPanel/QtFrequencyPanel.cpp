//
// Created by murray on 28/1/26.
//

#include "QtFrequencyPanel.h"
#include "ui_QtFrequencyPanel.h"

#include "QtBandReadout.h"
#include "VfoReadout/QtVfoReadout.h"

#include <QStyle>

#include "settings/RadioSettings.h"
#include "settings/bands/BandSelector.h"

QtFrequencyPanel::QtFrequencyPanel(QWidget* parent)
  : QWidget(parent)
  , ui(std::make_unique<Ui::QtFrequencyPanel>())
  , m_pSettingsSink(nullptr)
  , m_band1Readout(nullptr)
  , m_band2Readout(nullptr)
{
  ui->setupUi(this);
  setAttribute(Qt::WA_StyledBackground, true);
  initialiseLayout();
}

QtFrequencyPanel::~QtFrequencyPanel() = default;

void
QtFrequencyPanel::initialiseLayout()
{
  // Keep using the .ui gridLayout as a simple 2-row host.
  ui->gridLayout->setContentsMargins(0, 0, 0, 0);
  ui->gridLayout->setHorizontalSpacing(6);
  ui->gridLayout->setVerticalSpacing(2);
  ui->gridLayout->setColumnStretch(0, 1);
  ui->gridLayout->setColumnStretch(1, 1);

  m_band1Readout = new QtBandReadout(SplitBandId::One, this);
  // m_band1Readout->setObjectName("band1Row");
  ui->gridLayout->addWidget(m_band1Readout, 0, 0, 1, 2, Qt::AlignLeft);

  m_band2Readout = new QtBandReadout(SplitBandId::Two, this);
  // m_band2Readout->setObjectName("band2Row");
  ui->gridLayout->addWidget(m_band2Readout, 1, 0, 1, 2, Qt::AlignLeft);

  // Wire row-level actions to frequency-readout actions (which produce SettingUpdates).
  connect(m_band1Readout, &QtBandReadout::splitRequested, this, &QtFrequencyPanel::onSplitRequested);
  connect(m_band2Readout, &QtBandReadout::splitRequested, this, &QtFrequencyPanel::onSplitRequested);
  connect(m_band1Readout, &QtBandReadout::closeRequested, this, &QtFrequencyPanel::onCloseRequested);
  connect(m_band2Readout, &QtBandReadout::closeRequested, this, &QtFrequencyPanel::onCloseRequested);

  connect(m_band1Readout, &QtBandReadout::multiVfoActionRequested, this, &QtFrequencyPanel::onMultiVfoActionRequested);
  connect(m_band2Readout, &QtBandReadout::multiVfoActionRequested, this, &QtFrequencyPanel::onMultiVfoActionRequested);

  connect(m_band1Readout, &QtBandReadout::vfoTxActionRequested, this, &QtFrequencyPanel::onVfoTxActionRequested);
  connect(m_band2Readout, &QtBandReadout::vfoTxActionRequested, this, &QtFrequencyPanel::onVfoTxActionRequested);

  // connect(m_band1Readout, &QtBandReadout::txBandRequested, this, &QtFrequencyPanel::onTxBandClicked);
  // connect(m_band2Readout, &QtBandReadout::txBandRequested, this, &QtFrequencyPanel::onTxBandClicked);

  connect(m_band1Readout, &QtBandReadout::bandClicked, this, &QtFrequencyPanel::onBandClicked);
  connect(m_band2Readout, &QtBandReadout::bandClicked, this, &QtFrequencyPanel::onBandClicked);
  connect(m_band1Readout, &QtBandReadout::vfoClicked, this, &QtFrequencyPanel::onVfoClicked);
  connect(m_band2Readout, &QtBandReadout::vfoClicked, this, &QtFrequencyPanel::onVfoClicked);

  // // NEW: forward SettingUpdates from rows to the sink.
  // connect(m_band1Readout, &QtBandReadout::settingUpdateRequested, this, [this](SettingUpdate u) {
  //   if (m_pSettingsSink) m_pSettingsSink->applySettingUpdate(u);
  // });
  // connect(m_band2Readout, &QtBandReadout::settingUpdateRequested, this, [this](SettingUpdate u) {
  //   if (m_pSettingsSink) m_pSettingsSink->applySettingUpdate(u);
  // });

  m_band1Readout->setWidgetProperties(false);
  m_band2Readout->setWidgetProperties(false);
  QWidgetPropertySetter::setWidgetProperty(this, "role", "frequencyReadout", true);
}

void
QtFrequencyPanel::initialise(RadioSettings* pRadioSettings)
{
  applyRadioSettings(pRadioSettings, false);
}

void
QtFrequencyPanel::applyRadioSettings(RadioSettings* pRadioSettings, bool onlyIfChanged)
{
  if (pRadioSettings == nullptr) return;

  if (pRadioSettings->hasSettingChanged(RadioSettings::BAND)) {
    BandSelector& bandSelector = pRadioSettings->getBandSelector();
    if (bandSelector.isChanged()) {
      if (hasChangesOtherThanVfo(bandSelector)) {
        applyBandSelectorChange(bandSelector);
        applyFrequencyAndPipelineChanges(bandSelector, onlyIfChanged);
      } else {
        applyFrequencyChanges(bandSelector, onlyIfChanged);
      }
      // BandSettings* focusBand = bandSelector.getFocusBandSettings();
      // if (onlyIfChanged && focusBand != nullptr && focusBand->hasFocusVfoChanged()) {
      //   // Do the bare minimum for VFO changes otherwise Qt stuff bogs everything down.
      //   applyFrequencyChanges(bandSelector, onlyIfChanged);
      // } else {
      //
      // }
      //
    }
  } else if (pRadioSettings->hasSettingChanged(RadioSettings::PTT)) {
    setPttProperty(pRadioSettings->getPtt());
  }
}

bool
QtFrequencyPanel::hasChangesOtherThanVfo(BandSelector& bandSelector) const
{
  uint32_t bandSelectorChanges = BandSelector::REPLACE_FOCUS
    | BandSelector::SELECT_1
    | BandSelector::SELECT_2
    | BandSelector::FOCUS
    | BandSelector::SPLIT
    | BandSelector::TX_BAND
    | BandSelector::RX_BAND;
  if (bandSelector.hasSettingChanged(bandSelectorChanges)) {
    return true;
  }
  bandSelectorChanges = BandSelector::WITH_1
    | BandSelector::WITH_2
    | BandSelector::WITH_FOCUS;
  if (bandSelector.hasSettingChanged(bandSelectorChanges)) {
    BandSettings* bandSettings = bandSelector.getFocusBandSettings();
    uint32_t pipelineChanges = BandSettings::MULTI_PIPELINE
      | BandSettings::FOCUS_PIPELINE
      | BandSettings::TX_PIPELINE
      | BandSettings::WITH_TX_PIPELINE
      | BandSettings::CLOSE_PIPELINE;
    if (bandSettings->hasSettingChanged(pipelineChanges)) {
      return true;
    }
    // pipelineChanges = BandSettings::WITH_FOCUS_PIPELINE
  }
  return false;
}

void
QtFrequencyPanel::applyFrequencyChanges(BandSelector& bandSelector, bool onlyIfChanged)
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
QtFrequencyPanel::applyFrequencyAndPipelineChanges(BandSelector& bandSelector, bool onlyIfChanged)
{
  // Band 1
  if (bandSelector.hasBand(SplitBandId::One)) {
    const BandSettings* band1 = bandSelector.getBandSettings(SplitBandId::One);
    if (band1 != nullptr) {
      m_band1Readout->applyFrequencyChanges(band1, onlyIfChanged);
      m_band1Readout->applyPipelineChanges(band1, onlyIfChanged);
    }
  }

  // Band 2 (may be absent when not split)
  if (bandSelector.hasBand(SplitBandId::Two)) {
    const BandSettings* band2 = bandSelector.getBandSettings(SplitBandId::Two);
    if (band2 != nullptr) {
      m_band2Readout->applyFrequencyChanges(band2, onlyIfChanged);
      m_band2Readout->applyPipelineChanges(band2, onlyIfChanged);
    }
  }
}

void
QtFrequencyPanel::applyBandSelectorChange(BandSelector& bandSelector)
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
    // m_band1Readout->setTxButtonVisible(isSplit);
  }

  if (hasBand2) {
    const BandSettings* band2 = bandSelector.getBandSettings(SplitBandId::Two);
    m_band2Readout->applyBandSettings(band2, txBandName, rxBandName, focusBandName);
    // m_band2Readout->setTxButtonVisible(isSplit);
  }

  // Vertically center the single band by spanning both rows
  if (ui && ui->gridLayout && m_band1Readout && m_band2Readout) {
    ui->gridLayout->removeWidget(m_band1Readout);
    ui->gridLayout->removeWidget(m_band2Readout);

    if (hasBand1 && !hasBand2) {
      ui->gridLayout->addWidget(m_band1Readout, 0, 0, 2, 2, Qt::AlignVCenter | Qt::AlignLeft);
      ui->gridLayout->addWidget(m_band2Readout, 1, 0, 1, 2, Qt::AlignLeft); // hidden anyway
    } else {
      ui->gridLayout->addWidget(m_band1Readout, 0, 0, 1, 2, Qt::AlignLeft);
      ui->gridLayout->addWidget(m_band2Readout, 1, 0, 1, 2, Qt::AlignLeft);
    }
  }

  updateRowActionModes(hasBand1, hasBand2, isSplit);

  setIsSplitProperty(isSplit, true);

  ui->gridLayout->setRowStretch(0, hasBand1 ? 1 : 0);
  ui->gridLayout->setRowStretch(1, hasBand2 ? 1 : 0);
  ui->gridLayout->invalidate();
  updateGeometry();
}

void
QtFrequencyPanel::updateRowActionModes(bool hasBand1, bool hasBand2, bool isSplit)
{
  // With the row widget, the gutters are fixed and buttons are icon-only,
  // so there is no horizontal jostling; we just change the action mode.

  if (!m_band1Readout || !m_band2Readout) return;

  if (isSplit && hasBand1 && hasBand2) {
    m_band1Readout->setBandActionMode(QtBandReadout::BandAction::Close);
    m_band2Readout->setBandActionMode(QtBandReadout::BandAction::Close);
    return;
  }

  // Not split: Band 1 is the only active row => it offers Split.
  if (hasBand1 && !hasBand2) {
    m_band1Readout->setBandActionMode(QtBandReadout::BandAction::Split);
    m_band2Readout->setBandActionMode(QtBandReadout::BandAction::Disabled);
    return;
  }

  // Fallback: disable actions (should be rare)
  m_band1Readout->setBandActionMode(QtBandReadout::BandAction::Disabled);
  m_band2Readout->setBandActionMode(QtBandReadout::BandAction::Disabled);
}

void
QtFrequencyPanel::onSplitRequested(SplitBandId /*whichBand*/)
{
  if (m_pSettingsSink == nullptr) return;

  SettingUpdatePath splitPath({RadioSettings::BAND, BandSelector::SPLIT});
  SettingUpdate splitSetting(splitPath, true, SettingUpdate::Meaning::VALUE);
  m_pSettingsSink->applySettingUpdate(splitSetting);
}

void
QtFrequencyPanel::onCloseRequested(SplitBandId whichBand)
{
  if (m_pSettingsSink == nullptr) return;

  BandSelector::Features select =
    whichBand == SplitBandId::One ? BandSelector::SELECT_1 : BandSelector::SELECT_2;

  SettingUpdatePath bandPath({RadioSettings::BAND, static_cast<uint32_t>(select)});
  SettingUpdate bandSetting(bandPath, "", SettingUpdate::Meaning::VALUE); // Empty band name closes it
  m_pSettingsSink->applySettingUpdate(bandSetting);
}

void
QtFrequencyPanel::onMultiVfoActionRequested(SplitBandId whichBand,
                                        VfoId whichVfo,
                                        MultiVfoAction action)
{
  uint32_t selectBand = whichBand == SplitBandId::One ? BandSelector::WITH_1 : BandSelector::WITH_2;

  switch (action) {
  case MultiVfoAction::Multi:
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
  case MultiVfoAction::Close:
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
QtFrequencyPanel::onVfoTxActionRequested(SplitBandId whichBand,
                            VfoId whichVfo,
                            VfoTxAction action)
{
  uint32_t selectBand = whichBand == SplitBandId::One ? BandSelector::WITH_1 : BandSelector::WITH_2;
  if (action == VfoTxAction::Tx) {
    SettingUpdatePath bandPath({
        RadioSettings::BAND,
        selectBand,
        BandSettings::TX_PIPELINE
      });
    SettingUpdate bandSetting(bandPath, whichVfo, SettingUpdate::Meaning::VALUE);
    m_pSettingsSink->applySettingUpdate(bandSetting);
  }

  switch (action) {
  case VfoTxAction::Tx:
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
QtFrequencyPanel::setPttProperty(bool ptt, bool repolish)
{
  m_band1Readout->setPttProperty(ptt, false);
  m_band2Readout->setPttProperty(ptt, false);
  QWidgetPropertySetter::setWidgetProperty(this, "ptt", ptt, repolish);
}

void
QtFrequencyPanel::setIsSplitProperty(bool isSplit, bool repolish)
{
  if (m_band1Readout != nullptr) m_band1Readout->setIsBandSplitProperty(isSplit, false);
  if (m_band2Readout != nullptr) m_band2Readout->setIsBandSplitProperty(isSplit, false);
  QWidgetPropertySetter::setWidgetProperty(this, "isSplit", isSplit, repolish);
}

void
QtFrequencyPanel::onBandClicked(SplitBandId whichBand)
{
  if (m_pSettingsSink == nullptr) return;

  SettingUpdatePath path({RadioSettings::BAND, BandSelector::FOCUS});
  SettingUpdate u(path, whichBand, SettingUpdate::Meaning::VALUE);
  m_pSettingsSink->applySettingUpdate(u);
}

void
QtFrequencyPanel::onTxBandClicked(SplitBandId whichBand)
{
  if (m_pSettingsSink == nullptr) return;

  SettingUpdatePath path({RadioSettings::BAND, BandSelector::TX_BAND});
  SettingUpdate update(path, whichBand, SettingUpdate::Meaning::VALUE);
  m_pSettingsSink->applySettingUpdate(update);
}

void
QtFrequencyPanel::onVfoClicked(SplitBandId whichBand, VfoId whichVfo)
{
  if (m_pSettingsSink == nullptr) return;

  SettingUpdate update = RadioSettings::makeBandSetFocusPipelineUpdate(whichBand, whichVfo);
  m_pSettingsSink->applySettingUpdate(update);
}