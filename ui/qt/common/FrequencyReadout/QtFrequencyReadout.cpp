//
// Created by murray on 28/1/26.
//

#include "QtFrequencyReadout.h"
#include "ui_QtFrequencyReadout.h"
#include "../QtNumberReadout.h"
#include <QStyle>

#include "settings/RadioSettings.h"
#include "settings/bands/BandSelector.h"


QtFrequencyReadout::QtFrequencyReadout(QWidget* parent)
  : QWidget(parent)
  , ui(std::make_unique<Ui::QtFrequencyReadout>())
  , m_pReadoutBandA_VfoA(new QtNumberReadout(this))
  , m_pReadoutBandA_VfoB(new QtNumberReadout(this))
  , m_pReadoutBandB_VfoA(new QtNumberReadout(this))
  , m_pReadoutBandB_VfoB(new QtNumberReadout(this))
  , m_pBandARow(nullptr)
  , m_pBandBRow(nullptr)
{
  ui->setupUi(this);
  initialiseGridLayout();
}

QtFrequencyReadout::~QtFrequencyReadout() = default;

void
QtFrequencyReadout::initialiseGridLayout()
{
  ui->gridLayout->setContentsMargins(0, 0, 0, 0);
  ui->gridLayout->setHorizontalSpacing(6);
  ui->gridLayout->setVerticalSpacing(2);

  m_pReadoutBandA_VfoA->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_pReadoutBandA_VfoB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_pReadoutBandB_VfoA->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_pReadoutBandB_VfoB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  m_pBandARow = new QWidget(this);
  m_pBandARow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_pBandARow->setObjectName("bandARow");
  auto* bandALayout = new QHBoxLayout(m_pBandARow);
  bandALayout->setContentsMargins(0, 0, 0, 0);
  bandALayout->addWidget(m_pReadoutBandA_VfoA);
  bandALayout->addWidget(m_pReadoutBandA_VfoB);
  ui->gridLayout->addWidget(m_pBandARow, 0, 0, 1, 2);

  m_pBandBRow = new QWidget(this);
  m_pBandBRow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_pBandBRow->setObjectName("bandBRow");
  auto* bandBLayout = new QHBoxLayout(m_pBandBRow);
  bandBLayout->setContentsMargins(0, 0, 0, 0);
  bandBLayout->addWidget(m_pReadoutBandB_VfoA);
  bandBLayout->addWidget(m_pReadoutBandB_VfoB);
  ui->gridLayout->addWidget(m_pBandBRow, 1, 0, 1, 2);
}

void
QtFrequencyReadout::initialise(RadioSettings* pRadioSettings)
{
  applyStyleContext(m_pReadoutBandA_VfoA, "A", "A", true, true, true, false);
  applyStyleContext(m_pReadoutBandA_VfoB, "A", "B", false, false, false, false);
  applyStyleContext(m_pReadoutBandB_VfoA, "B", "B", false, false, false, false);
  applyStyleContext(m_pReadoutBandB_VfoB, "B", "B", false, false, false, false);

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
  applyFrequencyChanges(m_pReadoutBandB_VfoA, bandBSettings->getRxPipelineSettings(0), onlyIfChanged);
  applyFrequencyChanges(m_pReadoutBandB_VfoB, bandBSettings->getRxPipelineSettings(1), onlyIfChanged);
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

  // --- Show/hide entire rows ---
  const bool showBandARow = show_bandA_vfoA || show_bandA_vfoB;
  const bool showBandBRow = show_bandB_vfoA || show_bandB_vfoB;

  m_pBandARow->setVisible(showBandARow);
  m_pBandBRow->setVisible(showBandBRow);

  // --- Make the remaining visible cell(s) expand within each row widget ---
  applyRowStretch(m_pBandARow, m_pReadoutBandA_VfoA, show_bandA_vfoA,
                              m_pReadoutBandA_VfoB, show_bandA_vfoB);
  applyRowStretch(m_pBandBRow, m_pReadoutBandB_VfoA, show_bandB_vfoA,
                              m_pReadoutBandB_VfoB, show_bandB_vfoB);

  // --- Grid stretch: if only one row is visible, let it take the space ---
  ui->gridLayout->setRowStretch(0, showBandARow ? 1 : 0);
  ui->gridLayout->setRowStretch(1, showBandBRow ? 1 : 0);

  // Since each row widget spans both columns, column stretch is mostly irrelevant,
  // but keeping it "elastic" doesn't hurt:
  ui->gridLayout->setColumnStretch(0, 1);
  ui->gridLayout->setColumnStretch(1, 1);

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
  if (left)  h->setStretchFactor(left,  showLeft  ? 1 : 0);
  if (right) h->setStretchFactor(right, showRight ? 1 : 0);
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
    props[1] = P("focusVfo", focusVfo);
    props[3] = P("focusRx", focusRx);
    props[4] = P("focusTx", focusTx);
    props[5] = P("ptt", ptt);

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