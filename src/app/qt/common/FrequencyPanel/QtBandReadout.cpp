#include "QtBandReadout.h"

#include <qevent.h>
#include <QHBoxLayout>
#include <QToolButton>
#include <QIcon>
#include <QStyle>

#include "VfoReadout/QtVfoReadout.h"

#include "../QWidgetPropertySetter.h"

QtBandReadout::QtBandReadout(SplitBandId bandIndex, QWidget* parent)
  : QWidget(parent)
  , m_splitBandId(bandIndex)
{
  setAttribute(Qt::WA_StyledBackground, true);
  setAttribute(Qt::WA_AcceptTouchEvents, true);
  initialise();
}

void
QtBandReadout::initialise()
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  setMinimumWidth(0);

  buildUi();

  connect(m_addRemoveBandButton, &QToolButton::clicked, this, &QtBandReadout::onLeftActionClicked);

  // VFO badge clicks map to row-level split/close requests.
  connect(m_vfoA, &QtVfoReadout::multiVfoActionRequested, this, [this](MultiVfoAction mode) {
    emit multiVfoActionRequested(m_splitBandId, VfoId::A, mode);
  });

  connect(m_vfoB, &QtVfoReadout::multiVfoActionRequested, this, [this](MultiVfoAction mode) {
    emit multiVfoActionRequested(m_splitBandId, VfoId::B, mode);
  });

  connect(m_vfoA, &QtVfoReadout::vfoTxActionRequested, this, [this](VfoTxAction mode) {
    emit vfoTxActionRequested(m_splitBandId, VfoId::A, mode);
  });

  connect(m_vfoB, &QtVfoReadout::vfoTxActionRequested, this, [this](VfoTxAction mode) {
    emit vfoTxActionRequested(m_splitBandId, VfoId::B, mode);
  });


  // VFO background clicks select focus
  connect(m_vfoA, &QtVfoReadout::clicked, this, [this](VfoId id) {
    emit bandClicked(m_splitBandId);
    emit vfoClicked(m_splitBandId, id);
  });
  connect(m_vfoB, &QtVfoReadout::clicked, this, [this](VfoId id) {
    emit bandClicked(m_splitBandId);
    emit vfoClicked(m_splitBandId, id);
  });

  // Forward SettingUpdates from VFO mini toolbars directly to the sink (if set).
  const auto forwardUpdate = [this](QVector<SettingUpdate>& updates) {
    if (m_pSettingsSink != nullptr) {
      m_pSettingsSink->applySettingUpdates(updates.data(), updates.size());
    }
  };
  connect(m_vfoA, &QtVfoReadout::settingUpdateRequested, this, forwardUpdate);
  connect(m_vfoB, &QtVfoReadout::settingUpdateRequested, this, forwardUpdate);


  // Keep icons consistent within the fixed-width gutters.
  const int iconPx = kActionSlotWidthPx - 6; // a little padding
  m_addRemoveBandButton->setIconSize(QSize(iconPx, iconPx));

  // Defaults: stable gutters exist, but actions start disabled until parent configures them.
  setBandActionMode(BandAction::Disabled);

  setVfoBVisible(false);

  setWidgetProperties();
}

void
QtBandReadout::setWidgetProperties(bool repolish)
{
  // m_vfoA->setWidgetProperties(false);
  // m_vfoB->setWidgetProperties(false);
  QWidgetPropertySetter::setWidgetProperty(m_addRemoveBandButton, "role", "split");

  using P = WidgetProperty;
  static std::array<P, 2> props;

  props[0] = P("role", "bandReadout");
  props[1] = P("band", m_splitBandId == SplitBandId::One ? "one" : "two");

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

  // ---- Left cell: [action gutter] [VFO A]
  m_leftCell = new QWidget(this);
  m_leftCell->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  m_leftCell->setMinimumWidth(0);

  auto* leftLayout = new QHBoxLayout(m_leftCell);
  leftLayout->setContentsMargins(0, 0, 0, 0);
  leftLayout->setSpacing(6);

  // Action gutter: fixed width (computed), but EXPANDS vertically so Split can be centered in the full row height.
  m_actionGutter = new QWidget(m_leftCell);
  m_actionGutter->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  auto* gutterLayout = new QVBoxLayout(m_actionGutter);
  gutterLayout->setContentsMargins(0, 0, 0, 0);
  gutterLayout->setSpacing(4);

  // --- Split button (centered vertically + horizontally)
  m_addRemoveBandButton = new QToolButton(m_actionGutter);
  m_addRemoveBandButton->setAutoRaise(true);
  m_addRemoveBandButton->setCursor(Qt::PointingHandCursor);
  m_addRemoveBandButton->setFocusPolicy(Qt::NoFocus);
  m_addRemoveBandButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  m_addRemoveBandButton->setText(QStringLiteral("Split"));
  m_addRemoveBandButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  auto* splitCenterArea = new QWidget(m_actionGutter);
  splitCenterArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

  auto* splitCenterLayout = new QVBoxLayout(splitCenterArea);
  splitCenterLayout->setContentsMargins(0, 0, 0, 0);
  splitCenterLayout->setSpacing(0);
  splitCenterLayout->addStretch(1);
  splitCenterLayout->addWidget(m_addRemoveBandButton, 0, Qt::AlignHCenter | Qt::AlignVCenter);
  splitCenterLayout->addStretch(1);

  gutterLayout->addWidget(splitCenterArea, /*stretch*/ 1);

  // Compute gutter width so "Split" text can actually appear (but never smaller than the old fixed slot).
  updateActionGutterWidth();
  // m_addRemoveBandButton->ensurePolished();
  // actionGutter->setFixedWidth(std::max(kActionSlotWidthPx, m_addRemoveBandButton->sizeHint().width()));

  // --- VFO A
  m_vfoA = new QtVfoReadout(VfoId::A, m_leftCell);
  m_vfoA->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  m_vfoA->setBandId(m_splitBandId);

  // Put gutter next to VFO A
  leftLayout->addWidget(m_actionGutter, 0, Qt::AlignVCenter);
  leftLayout->addWidget(m_vfoA, 0, Qt::AlignTop);
  leftLayout->setStretch(0, 0);
  leftLayout->setStretch(1, 0);

  // ---- Right cell: [VFO B/placeholder]
  m_rightCell = new QWidget(this);
  m_rightCell->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  m_rightCell->setMinimumWidth(0);

  auto* rightLayout = new QHBoxLayout(m_rightCell);
  rightLayout->setContentsMargins(0, 0, 0, 0);
  rightLayout->setSpacing(6);

  m_rightStackHost = new QWidget(m_rightCell);
  m_rightStackHost->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  m_rightStackHost->setMinimumWidth(0);

  m_rightStack = new QStackedLayout(m_rightStackHost);
  m_rightStack->setContentsMargins(0, 0, 0, 0);

  m_vfoPlaceholder = new QWidget(m_rightStackHost);
  m_vfoPlaceholder->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  m_vfoPlaceholder->setMinimumWidth(0);
  m_vfoPlaceholder->setObjectName("vfoPlaceholder");

  m_vfoB = new QtVfoReadout(VfoId::B, m_rightStackHost);
  m_vfoB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  m_vfoB->setBandId(m_splitBandId);

  m_rightStack->addWidget(m_vfoB);            // index 0
  m_rightStack->addWidget(m_vfoPlaceholder);  // index 1

  rightLayout->addWidget(m_rightStackHost, 0, Qt::AlignTop);
  rightLayout->setStretch(0, 0);

  // ---- Add both cells to row
  m_rowLayout->addWidget(m_leftCell, 0, Qt::AlignTop);
  m_rowLayout->addWidget(m_rightCell, 0, Qt::AlignTop);
  m_rowLayout->setStretch(0, 0);
  m_rowLayout->setStretch(1, 0);

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

  // m_rightCell->setVisible(true);

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
  const bool isFocusBand = bandName == focusBandName;
  const bool isTxBand = bandName == txBandName;
  const bool isRxBand = bandName == rxBandName;

  setIsFocusBand(isFocusBand, false);
  setIsTxBand(isTxBand, false);
  QWidgetPropertySetter::repolishRecursively(this);

  const bool isMultiVfo = bandSettings->isMultiPipeline();
  setVfoBVisible(isMultiVfo);

  if (m_vfoA != nullptr) {
    m_vfoA->applyBandSettings(bandSettings, !isMultiVfo, isFocusBand, isTxBand);
  }

  if (m_vfoB != nullptr) {
    m_vfoB->applyBandSettings(bandSettings, false, isFocusBand, isTxBand);
  }
  applyPipelineChanges(bandSettings, /*onlyIfChanged*/ false);
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

void QtBandReadout::applyPipelineChanges(const BandSettings* bandSettings, bool onlyIfChanged)
{
  if (!bandSettings) return;

  const auto applyMode = [onlyIfChanged](QtVfoReadout* readout, const RxPipelineSettings* p) {
    if (!readout) return;

    if (!p) {
      readout->setModeText(QStringLiteral("Mode"));
      return;
    }

    // Only update on changes, if requested.
    if (onlyIfChanged && !p->hasSettingChanged(PipelineSettings::MODE)) {
      return;
    }

    readout->setModeText(QString::fromStdString(p->getMode().getName()));
  };

  applyMode(m_vfoA, bandSettings->getRxPipeline(PipelineId::A));
  applyMode(m_vfoB, bandSettings->getRxPipeline(PipelineId::B));
}

void QtBandReadout::updateActionGutterWidth()
{
  if (!m_actionGutter || !m_addRemoveBandButton) return;

  // Make sure stylesheet/font has been applied before measuring.
  m_addRemoveBandButton->ensurePolished();

  // Size hint already accounts for ToolButtonTextBesideIcon + current font/icon size.
  const int needed = m_addRemoveBandButton->sizeHint().width();

  // Keep at least the legacy slot width, but allow growth for readable text.
  const int w = std::max(kActionSlotWidthPx, needed);

  m_actionGutter->setFixedWidth(w);
}

void QtBandReadout::updateLeftButtonPresentation()
{
  if (!m_addRemoveBandButton) return;

  // Match the MiniVfoToolbar "A/B" behavior: text + icon that changes on active state
  const QIcon openIcon(":/ui/icons/solid/clone.svg");
  const QIcon closeIcon(":/ui/icons/solid/circle-xmark.svg");

  m_addRemoveBandButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  m_addRemoveBandButton->setText(QStringLiteral("Split"));

  switch (m_addRemoveBandAction) {
  case BandAction::Split:
    m_addRemoveBandButton->setIcon(openIcon);
    m_addRemoveBandButton->setToolTip("Split");
    m_addRemoveBandButton->setEnabled(true);
    break;

  case BandAction::Close:
    m_addRemoveBandButton->setIcon(closeIcon);
    m_addRemoveBandButton->setToolTip("Split (Close)");
    m_addRemoveBandButton->setEnabled(true);
    break;

  case BandAction::Disabled:
    m_addRemoveBandButton->setIcon(openIcon);
    m_addRemoveBandButton->setToolTip(QString());
    m_addRemoveBandButton->setEnabled(false);
    break;
  }

  m_addRemoveBandButton->style()->unpolish(m_addRemoveBandButton);
  m_addRemoveBandButton->style()->polish(m_addRemoveBandButton);
  m_addRemoveBandButton->update();
  updateActionGutterWidth();
}

void QtBandReadout::onLeftActionClicked()
{
  switch (m_addRemoveBandAction) {
    case BandAction::Split:
      emit splitRequested(m_splitBandId);
      break;
    case BandAction::Close:
      emit closeRequested(m_splitBandId);
      break;
    case BandAction::Disabled:
      break;
  }
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

    emit bandClicked(m_splitBandId);
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

        emit bandClicked(m_splitBandId);
        e->accept();
        return true;
      }
    }
  }

  return QWidget::event(e);
}