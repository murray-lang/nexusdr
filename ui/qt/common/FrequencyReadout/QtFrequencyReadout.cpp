//
// Created by murray on 28/1/26.
//

#include "QtFrequencyReadout.h"
#include "ui_QtFrequencyReadout.h"
#include "../QtNumberReadout.h"
#include <QStyle>
#include <QStackedLayout>
#include <QToolButton>

#include "settings/RadioSettings.h"
#include "settings/bands/BandSelector.h"


QtFrequencyReadout::QtFrequencyReadout(QWidget* parent)
  : QWidget(parent)
  , ui(std::make_unique<Ui::QtFrequencyReadout>())
  , m_pSettingsSink(nullptr)
  , m_pReadoutBandA_VfoA(new QtNumberReadout(this))
  , m_pReadoutBandA_VfoB(new QtNumberReadout(this))
  , m_pReadoutBandB_VfoA(new QtNumberReadout(this))
  , m_pReadoutBandB_VfoB(new QtNumberReadout(this))
  , m_pBandARow(nullptr)
  , m_pBandBRow(nullptr)
  , m_pBandA_LeftCell(nullptr)
  , m_pBandA_RightCell(nullptr)
  , m_pBandB_LeftCell(nullptr)
  , m_pBandB_RightCell(nullptr)
  , m_pBandA_RightStack(nullptr)
  , m_pBandB_RightStack(nullptr)
  , m_pBandA_RightPlaceholder(nullptr)
  , m_pBandB_RightPlaceholder(nullptr)
  , m_pBandA_ActionButton(nullptr)
  , m_pBandB_ActionButton(nullptr)
{
  ui->setupUi(this);
  setAttribute(Qt::WA_StyledBackground, true);
  initialiseGridLayout();

  applyStyleContext(m_pReadoutBandA_VfoA, "A", "A", true, true, true, false);
  applyStyleContext(m_pReadoutBandA_VfoB, "A", "B", false, false, false, false);
  applyStyleContext(m_pReadoutBandB_VfoA, "B", "A", false, false, false, false);
  applyStyleContext(m_pReadoutBandB_VfoB, "B", "B", false, false, false, false);
}

QtFrequencyReadout::~QtFrequencyReadout() = default;

void
QtFrequencyReadout::initialiseGridLayout()
{
  ui->gridLayout->setContentsMargins(0, 0, 0, 0);
  ui->gridLayout->setHorizontalSpacing(6);
  ui->gridLayout->setVerticalSpacing(2);
  ui->gridLayout->setColumnStretch(0, 1);
  ui->gridLayout->setColumnStretch(1, 1);

  m_pReadoutBandA_VfoA->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_pReadoutBandA_VfoB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_pReadoutBandB_VfoA->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_pReadoutBandB_VfoB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  // Build rows
  {
    RowWidgets bandA = createBandRow("bandARow", m_pReadoutBandA_VfoA, m_pReadoutBandA_VfoB);
    m_pBandARow = bandA.row;
    m_pBandA_LeftCell = bandA.leftCell;
    m_pBandA_RightCell = bandA.rightCell;
    m_pBandA_RightStack = bandA.rightStack;
    m_pBandA_RightPlaceholder = bandA.rightPlaceholder;
    m_pBandA_ActionButton = bandA.actionButton;
    ui->gridLayout->addWidget(m_pBandARow, 0, 0, 1, 2);
  }

  {
    RowWidgets bandB = createBandRow("bandBRow", m_pReadoutBandB_VfoA, m_pReadoutBandB_VfoB);
    m_pBandBRow = bandB.row;
    m_pBandB_LeftCell = bandB.leftCell;
    m_pBandB_RightCell = bandB.rightCell;
    m_pBandB_RightStack = bandB.rightStack;
    m_pBandB_RightPlaceholder = bandB.rightPlaceholder;
    m_pBandB_ActionButton = bandB.actionButton;
    ui->gridLayout->addWidget(m_pBandBRow, 1, 0, 1, 2);
  }
}

QtFrequencyReadout::RowWidgets
QtFrequencyReadout::createBandRow(const QString& rowObjectName,
                                  QtNumberReadout* vfoA,
                                  QtNumberReadout* vfoB)
{
  RowWidgets rowWidgets;

  rowWidgets.row = new QWidget(this);
  rowWidgets.row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  rowWidgets.row->setMinimumWidth(0);
  rowWidgets.row->setObjectName(rowObjectName);

  auto* rowLayout = new QHBoxLayout(rowWidgets.row);
  rowLayout->setContentsMargins(0, 0, 0, 0);
  rowLayout->setSpacing(6);

  rowWidgets.leftCell = new QWidget(rowWidgets.row);
  rowWidgets.leftCell->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  rowWidgets.leftCell->setMinimumWidth(0);

  auto* leftLayout = new QHBoxLayout(rowWidgets.leftCell);
  leftLayout->setContentsMargins(0, 0, 0, 0);
  leftLayout->setSpacing(6);

  rowWidgets.actionButton = new QToolButton(rowWidgets.leftCell);
  rowWidgets.actionButton->setAutoRaise(true);
  rowWidgets.actionButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
  rowWidgets.actionButton->setObjectName(rowObjectName + "actionButton");
  rowWidgets.actionButton->setVisible(false); // controlled by updateRowActionButtons()
  leftLayout->addWidget(rowWidgets.actionButton);

  leftLayout->addWidget(vfoA);

  rowWidgets.rightCell = new QWidget(rowWidgets.row);
  rowWidgets.rightCell->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  rowWidgets.rightCell->setMinimumWidth(0);

  rowWidgets.rightStack = new QStackedLayout(rowWidgets.rightCell);
  rowWidgets.rightStack->setContentsMargins(0, 0, 0, 0);

  rowWidgets.rightPlaceholder = new QWidget(rowWidgets.rightCell);
  rowWidgets.rightPlaceholder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  rowWidgets.rightPlaceholder->setMinimumWidth(0);
  rowWidgets.rightPlaceholder->setObjectName("vfoPlaceholder");

  rowWidgets.rightStack->addWidget(vfoB);
  rowWidgets.rightStack->addWidget(rowWidgets.rightPlaceholder);

  rowLayout->addWidget(rowWidgets.leftCell);
  rowLayout->addWidget(rowWidgets.rightCell);

  rowLayout->setStretchFactor(rowWidgets.leftCell, 1);
  rowLayout->setStretchFactor(rowWidgets.rightCell, 1);

  return rowWidgets;
}

void
QtFrequencyReadout::initialise(RadioSettings* pRadioSettings)
{
  applyRadioSettings(pRadioSettings, false);
}

void
QtFrequencyReadout::applyRadioSettings(RadioSettings* pRadioSettings, bool onlyIfChanged)
{
  if (pRadioSettings->hasSettingChanged(RadioSettings::BAND)) {
    BandSelector& bandSelector = pRadioSettings->getBandSelector();
    if (bandSelector.isChanged()) {
      applyBandSelectorChange(bandSelector);
      applyFrequencyChanges(bandSelector, onlyIfChanged);
    }
  } else if (pRadioSettings->hasSettingChanged(RadioSettings::PTT)) {
    applyPtt(pRadioSettings->getPtt());
  }
}

void
QtFrequencyReadout::applyFrequencyChanges(BandSelector& bandSelector, bool onlyIfChanged)
{
  BandSettings* bandASettings = bandSelector.getBandASettings();
  applyFrequencyChanges(m_pReadoutBandA_VfoA, bandASettings->getRxPipelineSettings(0), onlyIfChanged);
  applyFrequencyChanges(m_pReadoutBandA_VfoB, bandASettings->getRxPipelineSettings(1), onlyIfChanged);

  BandSettings* bandBSettings = bandSelector.getBandBSettings();
  if (bandBSettings != nullptr) {
    applyFrequencyChanges(m_pReadoutBandB_VfoA, bandBSettings->getRxPipelineSettings(0), onlyIfChanged);
    applyFrequencyChanges(m_pReadoutBandB_VfoB, bandBSettings->getRxPipelineSettings(1), onlyIfChanged);
  }
}

void
QtFrequencyReadout::applyFrequencyChanges(
  QtNumberReadout* readout,
  const RxPipelineSettings* rxPipelineSettings,
  bool onlyIfChanged
  )
{
  if (rxPipelineSettings) {
    const RfSettings& rfSettings = rxPipelineSettings->getRfSettings();
    if (rfSettings.hasSettingChanged(RfSettings::VFO) || !onlyIfChanged) {
      readout->setValue(rfSettings.getVfo());
    }
  }
}

void
QtFrequencyReadout::applyBandSelectorChange(BandSelector& bandSelector)
{
  BandSettings* bandASettings = bandSelector.getBandASettings();
  BandSettings* bandBSettings = bandSelector.getBandBSettings();
  const std::string& txBandName = bandSelector.getTxBandName();
  const std::string& rxBandName = bandSelector.getRxBandName();
  const std::string& focusBandName = bandSelector.getFocusBandName();

  bool show_bandA_vfoA = false;
  bool show_bandA_vfoB = false;
  bool show_bandB_vfoA = false;
  bool show_bandB_vfoB = false;

  m_pBandARow->setProperty("focusBand", focusBandName == bandASettings->getBandName());
  m_pBandARow->style()->unpolish(m_pBandARow);
  m_pBandARow->style()->polish(m_pBandARow);
  m_pBandARow->update();

  applyBandSettings(
      m_pReadoutBandA_VfoA,
      m_pReadoutBandA_VfoB,
      bandASettings,
      txBandName,
      rxBandName,
      focusBandName,
      &show_bandA_vfoA,
      &show_bandA_vfoB
    );


  if (bandSelector.isSplit()) {
    m_pBandBRow->setProperty("focusBand", focusBandName == bandBSettings->getBandName());
    m_pBandBRow->style()->unpolish(m_pBandBRow);
    m_pBandBRow->style()->polish(m_pBandBRow);
    m_pBandBRow->update();

    applyBandSettings(
      m_pReadoutBandB_VfoA,
      m_pReadoutBandB_VfoB,
      bandBSettings,
      txBandName,
      rxBandName,
      focusBandName,
      &show_bandB_vfoA,
      &show_bandB_vfoB
    );


  }
  showHide(show_bandA_vfoA, show_bandA_vfoB, show_bandB_vfoA, show_bandB_vfoB);
  const bool showBandARow = show_bandA_vfoA || show_bandA_vfoB;
  const bool showBandBRow = show_bandB_vfoA || show_bandB_vfoB;
  updateRowActionButtons(showBandARow, showBandBRow, bandSelector.isSplit());
}

void
QtFrequencyReadout::updateRowActionButtons(bool showBandARow, bool showBandBRow, bool isSplit)
{
  if (!m_pBandA_ActionButton || !m_pBandB_ActionButton) {
    // First call happens after initialiseGridLayout() has created rows; but be defensive.
  }

  const int visibleRows = (showBandARow ? 1 : 0) + (showBandBRow ? 1 : 0);
  const bool singleRow = (visibleRows == 1);

  auto setupAsSplit = [](QToolButton* b) {
    if (!b) return;
    b->setText("Split");
    b->setToolTip("Split");
    b->setVisible(true);
  };

  auto setupAsClose = [](QToolButton* b) {
    if (!b) return;
    b->setText("×");
    b->setToolTip("Close");
    b->setVisible(true);
  };

  // Reset connections (avoid stacking multiple connects as settings update)
  if (m_pBandA_ActionButton) m_pBandA_ActionButton->disconnect(this);
  if (m_pBandB_ActionButton) m_pBandB_ActionButton->disconnect(this);

  if (isSplit) {
    if (showBandARow) {
      setupAsClose(m_pBandA_ActionButton);
      connect(m_pBandA_ActionButton, &QToolButton::clicked, this, &QtFrequencyReadout::onCloseBandA);
    } else if (m_pBandA_ActionButton) {
      m_pBandA_ActionButton->setVisible(false);
    }

    if (showBandBRow) {
      setupAsClose(m_pBandB_ActionButton);
      connect(m_pBandB_ActionButton, &QToolButton::clicked, this, &QtFrequencyReadout::onCloseBandB);
    } else if (m_pBandB_ActionButton) {
      m_pBandB_ActionButton->setVisible(false);
    }
    return;
  }

  // Not split: only show Split when exactly one row is visible
  if (singleRow) {
    if (showBandARow) {
      setupAsSplit(m_pBandA_ActionButton);
      if (m_pBandB_ActionButton) m_pBandB_ActionButton->setVisible(false);
      connect(m_pBandA_ActionButton, &QToolButton::clicked, this, &QtFrequencyReadout::onSplitClicked);
    } else {
      setupAsSplit(m_pBandB_ActionButton);
      if (m_pBandA_ActionButton) m_pBandA_ActionButton->setVisible(false);
      connect(m_pBandB_ActionButton, &QToolButton::clicked, this, &QtFrequencyReadout::onSplitClicked);
    }
  } else {
    if (m_pBandA_ActionButton) m_pBandA_ActionButton->setVisible(false);
    if (m_pBandB_ActionButton) m_pBandB_ActionButton->setVisible(false);
  }
}

void
QtFrequencyReadout::onSplitClicked()
{
  if (m_pSettingsSink != nullptr) {
    SettingUpdatePath splitPath({RadioSettings::BAND, BandSelector::SPLIT});
    SettingUpdate splitSetting(splitPath, true, SettingUpdate::Meaning::VALUE);
    m_pSettingsSink->applySettingUpdate(splitSetting);
  }
}

void
QtFrequencyReadout::onCloseBandA()
{

}

void
QtFrequencyReadout::onCloseBandB()
{

}

void
QtFrequencyReadout::applyBandSettings(
    QtNumberReadout* pVfoReadoutA,
    QtNumberReadout* pVfoReadoutB,
    const BandSettings* bandSettings,
    const std::string& txBandName,
    const std::string& rxBandName,
    const std::string& focusBandName,
    bool* pShowVfoA,
    bool* pShowVfoB
  )
{
  bool isFocusBand = bandSettings->getBandName() == focusBandName;
  bool isTxBand = bandSettings->getBandName() == txBandName;
  bool isRxBand = bandSettings->getBandName() == rxBandName;
  bool haveVfoB = bandSettings->getNumRxPipelines() > 1;
  *pShowVfoA = true;
  *pShowVfoB = haveVfoB;
  if (isFocusBand) {
    uint32_t focusVfoIndex = bandSettings->getFocusRxPipeline();
    uint32_t txVfoIndex = bandSettings->getRxPipelineTrackedByTx();
    applyFocus(pVfoReadoutA, focusVfoIndex == 0, txVfoIndex == 0 && isTxBand, txVfoIndex == 0 && isFocusBand);
    applyFocus(pVfoReadoutB, focusVfoIndex == 1, txVfoIndex == 1 && isTxBand, txVfoIndex == 1 && isFocusBand);
  } else {
    applyFocus(pVfoReadoutA, false, false, false);
    applyFocus(pVfoReadoutB, false, false, false);
  }
}

void
QtFrequencyReadout::showHide(bool show_bandA_vfoA, bool show_bandA_vfoB, bool show_bandB_vfoA, bool show_bandB_vfoB)
{
  m_pReadoutBandA_VfoA->setVisible(show_bandA_vfoA);
  m_pReadoutBandA_VfoB->setVisible(show_bandA_vfoB);
  m_pReadoutBandB_VfoA->setVisible(show_bandB_vfoA);
  m_pReadoutBandB_VfoB->setVisible(show_bandB_vfoB);

  const bool showBandARow = show_bandA_vfoA || show_bandA_vfoB;
  const bool showBandBRow = show_bandB_vfoA || show_bandB_vfoB;

  m_pBandARow->setVisible(showBandARow);
  m_pBandBRow->setVisible(showBandBRow);

  // Two-column mode if either band has VFO B
  const bool twoColumnMode = show_bandA_vfoB || show_bandB_vfoB;

  // In two-column mode, keep the right cell visible for BOTH rows,
  // and show either VFO B or an expanding placeholder so the column exists.
  if (twoColumnMode) {
    m_pBandA_RightCell->setVisible(true);
    m_pBandB_RightCell->setVisible(true);

    m_pBandA_RightStack->setCurrentWidget(show_bandA_vfoB ? static_cast<QWidget*>(m_pReadoutBandA_VfoB)
                                                          : static_cast<QWidget*>(m_pBandA_RightPlaceholder));
    m_pBandB_RightStack->setCurrentWidget(show_bandB_vfoB ? static_cast<QWidget*>(m_pReadoutBandB_VfoB)
                                                          : static_cast<QWidget*>(m_pBandB_RightPlaceholder));

    if (auto* hA = qobject_cast<QHBoxLayout*>(m_pBandARow->layout())) {
      hA->setStretch(0, 1);
      hA->setStretch(1, 1);
    }
    if (auto* hB = qobject_cast<QHBoxLayout*>(m_pBandBRow->layout())) {
      hB->setStretch(0, 1);
      hB->setStretch(1, 1);
    }
  } else {
    // One-column stretched mode ONLY if both bands are single-VFO:
    // hide the right cell entirely so left cell takes full width.
    m_pBandA_RightCell->setVisible(false);
    m_pBandB_RightCell->setVisible(false);

    if (auto* hA = qobject_cast<QHBoxLayout*>(m_pBandARow->layout())) {
      hA->setStretch(0, 1);
      hA->setStretch(1, 0);
    }
    if (auto* hB = qobject_cast<QHBoxLayout*>(m_pBandBRow->layout())) {
      hB->setStretch(0, 1);
      hB->setStretch(1, 0);
    }
  }

  ui->gridLayout->setRowStretch(0, showBandARow ? 1 : 0);
  ui->gridLayout->setRowStretch(1, showBandBRow ? 1 : 0);

  ui->gridLayout->invalidate();
  updateGeometry();
}

void
QtFrequencyReadout::applyRowStretch(
    QWidget* row,
    QWidget* left,
    bool showLeft,
    QWidget* right,
    bool showRight
    ) const
{
  if (!row) return;

  auto* h = qobject_cast<QHBoxLayout*>(row->layout());
  if (!h) return;

  // Only visible widgets get stretch=1, hidden get stretch=0.
  // - if one visible: it gets all space (1 vs 0)
  // - if both visible: they share (1 vs 1)
  if (left) {
    h->setStretchFactor(left,  showLeft ? 1 : 0);
  }
  if (right) {
    h->setStretchFactor(right, showRight ? 1 : 0);
  }
}

void
QtFrequencyReadout::applyStyleContext(
  QtNumberReadout* w,
  const char* band,
  const char* vfo,
  bool focusVfo,
  bool focusRx,
  bool focusTx,
  bool ptt)
{
  if (w) {
    using P = QtNumberReadout::StyleProperty;
    static std::array<P, 7> props;

    props[0] = P("role", std::string("frequencyReadout"));
    props[1] = P("vfo", vfo);        // "A" or "B"
    props[2] = P("band", band);       // "A" or "B" (split side)
    props[3] = P("focusVfo", focusVfo);
    props[4] = P("focusRx", focusRx);
    props[5] = P("focusTx", focusTx);
    props[6] = P("ptt", ptt);

    w->applyStyleProperties(props);
  }
}

void
QtFrequencyReadout::applyPtt(bool ptt)
{
  applyPtt(m_pReadoutBandA_VfoA, ptt);
  applyPtt(m_pReadoutBandA_VfoB, ptt);
  applyPtt(m_pReadoutBandB_VfoA, ptt);
  applyPtt(m_pReadoutBandB_VfoB, ptt);
}

void
QtFrequencyReadout::applyPtt(QtNumberReadout* w, bool ptt)
{
  if (w != nullptr) {
    using P = QtNumberReadout::StyleProperty;
    const auto props = std::array<P, 1>{
      P{"ptt",      ptt},
    };
    w->applyStyleProperties(props);
  }
}

void
QtFrequencyReadout::applyFocus(QtNumberReadout* w, bool focusVfo, bool focusRx, bool focusTx)
{
  if (w != nullptr) {
    using P = QtNumberReadout::StyleProperty;
    const auto props = std::array<P, 3>{
      P{"focusVfo", focusVfo},
      P{"focusRx", focusRx},
      P{"focusTx", focusTx},
    };
    w->applyStyleProperties(props);
  }

}