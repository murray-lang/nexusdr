#include "QtBandReadout.h"

#include <qevent.h>
#include <QHBoxLayout>
#include <QToolButton>
#include <QIcon>
#include <QStyle>

#include "VfoReadout/QtVfoReadout.h"

#include "ui/qt/common/QWidgetPropertySetter.h"

QtBandReadout::QtBandReadout(SplitBandId bandIndex, QWidget* parent)
  : QWidget(parent)
  , m_bandIndex(bandIndex)
{
  setAttribute(Qt::WA_StyledBackground, true);
  setAttribute(Qt::WA_AcceptTouchEvents, true);
  initialise();
}

void
QtBandReadout::initialise()
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  setMinimumWidth(0);

  buildUi();

  connect(m_addRemoveBandButton, &QToolButton::clicked, this, &QtBandReadout::onLeftActionClicked);
  connect(m_txBandButton, &QToolButton::clicked, this, &QtBandReadout::onRightActionClicked);

  // VFO badge clicks map to row-level split/close requests.
  connect(m_vfoA, &QtVfoReadout::multiVfoActionRequested, this, [this](MultiVfoAction mode) {
    emit multiVfoActionRequested(m_bandIndex, VfoId::A, mode);
  });

  connect(m_vfoB, &QtVfoReadout::multiVfoActionRequested, this, [this](MultiVfoAction mode) {
    emit multiVfoActionRequested(m_bandIndex, VfoId::B, mode);
  });

  connect(m_vfoA, &QtVfoReadout::vfoTxActionRequested, this, [this](VfoTxAction mode) {
    emit vfoTxActionRequested(m_bandIndex, VfoId::A, mode);
  });

  connect(m_vfoB, &QtVfoReadout::vfoTxActionRequested, this, [this](VfoTxAction mode) {
    emit vfoTxActionRequested(m_bandIndex, VfoId::B, mode);
  });


  // VFO background clicks select focus
  connect(m_vfoA, &QtVfoReadout::clicked, this, [this](VfoId id) {
    emit bandClicked(m_bandIndex);
    emit vfoClicked(m_bandIndex, id);
  });
  connect(m_vfoB, &QtVfoReadout::clicked, this, [this](VfoId id) {
    emit bandClicked(m_bandIndex);
    emit vfoClicked(m_bandIndex, id);
  });

  // Icon-only buttons (no horizontal jostling due to text).
  m_addRemoveBandButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
  // m_rightButton->setToolButtonStyle(Qt::ToolButtonIconOnly);

  // Keep icons consistent within the fixed-width gutters.
  const int iconPx = kActionSlotWidthPx - 6; // a little padding
  m_addRemoveBandButton->setIconSize(QSize(iconPx, iconPx));
  // m_rightButton->setIconSize(QSize(iconPx, iconPx));

  // Defaults: stable gutters exist, but actions start disabled until parent configures them.
  setBandActionMode(BandAction::Disabled);
  setTxButtonVisible(true);
  setVfoBVisible(false);

  setWidgetProperties();
}

void
QtBandReadout::setWidgetProperties(bool repolish)
{
  // m_vfoA->setWidgetProperties(false);
  // m_vfoB->setWidgetProperties(false);

  using P = WidgetProperty;
  static std::array<P, 2> props;

  props[0] = P("role", "bandReadout");
  props[1] = P("band", m_bandIndex == SplitBandId::One ? "one" : "two");

  QWidgetPropertySetter::setWidgetProperties(this, props, repolish);
}

void
QtBandReadout::setPttProperty(bool ptt, bool repolish)
{
  m_vfoA->setPttProperty(ptt, false);
  m_vfoB->setPttProperty(ptt, false);
  QWidgetPropertySetter::setWidgetProperty(this, "ptt", ptt, repolish);
}

void
QtBandReadout::setIsBandSplitProperty(bool isBandSplit, bool repolish)
{
  QWidgetPropertySetter::setWidgetProperty(this, "isBandSplit", isBandSplit, repolish);
}

void QtBandReadout::buildUi()
{
  m_rowLayout = new QHBoxLayout(this);
  m_rowLayout->setContentsMargins(0, 0, 0, 0);
  m_rowLayout->setSpacing(6);

  // ---- Left cell: [fixed left slot] [VFO A expanding]
  m_leftCell = new QWidget(this);
  m_leftCell->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_leftCell->setMinimumWidth(0);

  auto* leftLayout = new QHBoxLayout(m_leftCell);
  leftLayout->setContentsMargins(0, 0, 0, 0);
  leftLayout->setSpacing(6);

  auto* leftSlot = new QWidget(m_leftCell);
  leftSlot->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  leftSlot->setFixedWidth(kActionSlotWidthPx); // Different from right slot

  auto* leftSlotLayout = new QHBoxLayout(leftSlot);
  leftSlotLayout->setContentsMargins(0, 0, 0, 0);
  leftSlotLayout->setSpacing(0); // Absent from right slot

  m_addRemoveBandButton = new QToolButton(leftSlot);
  m_addRemoveBandButton->setAutoRaise(true);
  m_addRemoveBandButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
  m_addRemoveBandButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_addRemoveBandButton->setCursor(Qt::PointingHandCursor);
  leftSlotLayout->addWidget(m_addRemoveBandButton);

  m_vfoA = new QtVfoReadout(VfoId::A, m_leftCell);
  m_vfoA->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  leftLayout->addWidget(leftSlot);
  leftLayout->addWidget(m_vfoA);
  leftLayout->setStretch(0, 0);
  leftLayout->setStretch(1, 1);

  // ---- Right cell: [VFO B/placeholder expanding] [fixed right slot]
  m_rightCell = new QWidget(this);
  m_rightCell->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_rightCell->setMinimumWidth(0);

  auto* rightLayout = new QHBoxLayout(m_rightCell);
  rightLayout->setContentsMargins(0, 0, 0, 0);
  rightLayout->setSpacing(6);

  m_rightStackHost = new QWidget(m_rightCell);
  m_rightStackHost->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_rightStackHost->setMinimumWidth(0);

  m_rightStack = new QStackedLayout(m_rightStackHost);
  m_rightStack->setContentsMargins(0, 0, 0, 0);

  m_vfoPlaceholder = new QWidget(m_rightStackHost);
  m_vfoPlaceholder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_vfoPlaceholder->setMinimumWidth(0);
  m_vfoPlaceholder->setObjectName("vfoPlaceholder");

  m_vfoB = new QtVfoReadout(VfoId::B,m_rightStackHost);
  m_vfoB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  m_rightStack->addWidget(m_vfoB);            // index 0
  m_rightStack->addWidget(m_vfoPlaceholder);  // index 1

  auto* rightSlot = new QWidget(m_rightCell);
  rightSlot->setFixedWidth(kActionSlotWidthPx);
  rightSlot->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

  auto* rightSlotLayout = new QHBoxLayout(rightSlot);
  rightSlotLayout->setContentsMargins(0, 0, 0, 0);
  rightSlotLayout->setSpacing(0);

  m_txBandButton = new QToolButton(rightSlot);
  m_txBandButton->setText("TX");
  m_txBandButton->setAutoRaise(true);
  // m_rightButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
  m_txBandButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_txBandButton->setCursor(Qt::PointingHandCursor);
  QWidgetPropertySetter::setWidgetProperty(m_txBandButton, "role", "tx", true);
  rightSlotLayout->addWidget(m_txBandButton);

  rightLayout->addWidget(m_rightStackHost);
  rightLayout->addWidget(rightSlot);
  rightLayout->setStretch(0, 1);
  rightLayout->setStretch(1, 0);

  // ---- Add both cells to row
  m_rowLayout->addWidget(m_leftCell);
  m_rowLayout->addWidget(m_rightCell);
  m_rowLayout->setStretch(0, 1);
  m_rowLayout->setStretch(1, 1);

  updateLeftButtonPresentation();
}

void QtBandReadout::setIsFocusBand(bool focus, bool repolish)
{
  QWidgetPropertySetter::setWidgetProperty(this, "focusBand", focus, repolish);
}

void QtBandReadout::setIsTxBand(bool isTx, bool repolish)
{
  QWidgetPropertySetter::setWidgetProperty(this, "txBand", isTx, repolish);
}

void QtBandReadout::setRowVisible(bool visible)
{
  setVisible(visible);
}

void QtBandReadout::setBandActionMode(BandAction mode)
{
  m_addRemoveBandAction = mode;
  updateLeftButtonPresentation();
}

void QtBandReadout::setVfoBVisible(bool visible)
{
  if (!m_rowLayout || !m_rightCell || !m_rightStack || !m_rightStackHost) return;

  m_rightCell->setVisible(true);

  if (visible) {
    m_rightStackHost->setVisible(true);
    m_rightStack->setCurrentIndex(0); // VFO B
    m_rowLayout->setStretch(0, 1);
    m_rowLayout->setStretch(1, 1);
  } else {
    m_rightStack->setCurrentIndex(1); // placeholder (mostly irrelevant while hidden)
    m_rightStackHost->setVisible(false);
    m_rowLayout->setStretch(0, 1);
    m_rowLayout->setStretch(1, 0);
  }

  updateGeometry();
  update();
}

void QtBandReadout::setTxButtonVisible(bool visible)
{
  if (!m_txBandButton) return;
  m_txBandButton->setVisible(visible);
}

void QtBandReadout::applyFrequencyChanges(const BandSettings* bandSettings, bool onlyIfChanged)
{
  if (!bandSettings) return;

  applyFrequencyChanges(m_vfoA, bandSettings->getRxPipeline(VfoId::A), onlyIfChanged);
  applyFrequencyChanges(m_vfoB, bandSettings->getRxPipeline(VfoId::B), onlyIfChanged);
}

void QtBandReadout::applyBandSettings(const BandSettings* bandSettings,
                                     const std::string& txBandName,
                                     const std::string& rxBandName,
                                     const std::string& focusBandName)
{
  if (!bandSettings) return;

  const auto& bandName = bandSettings->getBandName();
  // m_bandName = bandName;

  const bool isFocusBand = bandName == focusBandName;
  const bool isTxBand = bandName == txBandName;
  const bool isRxBand = bandName == rxBandName;
  const bool haveVfoB = bandSettings->isMulti();

  setIsFocusBand(isFocusBand, false);
  setIsTxBand(isTxBand, false);
  QWidgetPropertySetter::repolishRecursively(this);

  setVfoBVisible(haveVfoB);

  const VfoId focusVfoId = bandSettings->getFocusId();
  const VfoId txVfoId = bandSettings->getTxId();

  if (!haveVfoB) {
    m_vfoA->setMultiVfoAction(MultiVfoAction::Multi);
    m_vfoB->setMultiVfoAction(MultiVfoAction::None);
    m_vfoA->setVfoTxAction(VfoTxAction::None);
    m_vfoB->setVfoTxAction(VfoTxAction::None);
  } else {
    m_vfoA->setMultiVfoAction(MultiVfoAction::Close);
    m_vfoB->setMultiVfoAction(MultiVfoAction::Close);
    m_vfoA->setVfoTxAction(isTxBand ? VfoTxAction::Tx : VfoTxAction::None);
    m_vfoB->setVfoTxAction(isTxBand ? VfoTxAction::Tx : VfoTxAction::None);
  }


  m_vfoA->applyFocusAndTx(focusVfoId == VfoId::A, txVfoId == VfoId::A && isTxBand);
  m_vfoB->applyFocusAndTx(focusVfoId == VfoId::B, txVfoId == VfoId::B && isTxBand);
}

void QtBandReadout::applyFrequencyChanges(QtVfoReadout* readout,
                                         const RxPipelineSettings* rxPipelineSettings,
                                         bool onlyIfChanged)
{
  if (!readout || !rxPipelineSettings) return;

  const RfSettings& rfSettings = rxPipelineSettings->getRfSettings();
  if (rfSettings.hasSettingChanged(RfSettings::VFO) || !onlyIfChanged) {
    readout->setValue(rfSettings.getVfo());
  }
}

void QtBandReadout::updateLeftButtonPresentation()
{
  if (!m_addRemoveBandButton) return;

  const QIcon splitIcon(":/ui/icons/solid/clone.svg");
  const QIcon closeIcon(":/ui/icons/solid/circle-xmark.svg");

  m_addRemoveBandButton->setText(QString());
  m_addRemoveBandButton->setToolButtonStyle(Qt::ToolButtonIconOnly);

  switch (m_addRemoveBandAction) {
    case BandAction::Split:
      m_addRemoveBandButton->setIcon(splitIcon);
      m_addRemoveBandButton->setToolTip("Split");
      m_addRemoveBandButton->setEnabled(true);
      break;

    case BandAction::Close:
      m_addRemoveBandButton->setIcon(closeIcon);
      m_addRemoveBandButton->setToolTip("Close");
      m_addRemoveBandButton->setEnabled(true);
      break;

    case BandAction::Disabled:
      // Keep a stable icon footprint even when disabled (no jostling).
      m_addRemoveBandButton->setIcon(splitIcon);
      m_addRemoveBandButton->setToolTip(QString());
      m_addRemoveBandButton->setEnabled(false);
      break;
  }
}

void QtBandReadout::onLeftActionClicked()
{
  switch (m_addRemoveBandAction) {
    case BandAction::Split:
      emit splitRequested(m_bandIndex);
      break;
    case BandAction::Close:
      emit closeRequested(m_bandIndex);
      break;
    case BandAction::Disabled:
      break;
  }
}

void QtBandReadout::onRightActionClicked()
{
  emit txBandRequested(m_bandIndex);
}

void
QtBandReadout::mousePressEvent(QMouseEvent* e)
{
  if (!e) return;

  if (e->button() == Qt::LeftButton) {
    // Ignore clicks on the gutter buttons (they handle themselves).
    if (m_addRemoveBandButton) {
      const QPoint p = m_addRemoveBandButton->mapFrom(this, e->position().toPoint());
      if (m_addRemoveBandButton->isVisible() && m_addRemoveBandButton->rect().contains(p)) {
        QWidget::mousePressEvent(e);
        return;
      }
    }
    if (m_txBandButton) {
      const QPoint p = m_txBandButton->mapFrom(this, e->position().toPoint());
      if (m_txBandButton->isVisible() && m_txBandButton->rect().contains(p)) {
        QWidget::mousePressEvent(e);
        return;
      }
    }

    emit bandClicked(m_bandIndex);
    e->accept();
    return;
  }

  QWidget::mousePressEvent(e);
}

bool
QtBandReadout::event(QEvent* e)
{
  if (!e) return QWidget::event(e);

  if (e->type() == QEvent::TouchEnd) {
    auto* te = dynamic_cast<QTouchEvent*>(e);
    const auto& points = te->points();
    if (!points.isEmpty()) {
      const QPointF pos = points.first().position();
      if (rect().contains(pos.toPoint())) {
        // Let VFO widgets / buttons deal with their own touches.
        // If the touch ends on a child widget, don't treat it as a row click.
        if (childAt(pos.toPoint()) != nullptr) {
          return QWidget::event(e);
        }

        emit bandClicked(m_bandIndex);
        e->accept();
        return true;
      }
    }
  }

  return QWidget::event(e);
}