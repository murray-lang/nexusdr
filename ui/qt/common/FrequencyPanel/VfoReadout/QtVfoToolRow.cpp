//
// Created by murray on 9/2/26.
//

#include "QtVfoToolRow.h"

#include <QAbstractButton>
#include <QHBoxLayout>
#include <QIcon>
#include <QToolButton>
#include <QStyle>

#include "ui/qt/common/QWidgetPropertySetter.h"

QtVfoToolRow::QtVfoToolRow(QWidget* parent)
  : QWidget(parent)
{
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  buildUi();
  updateMultiVfoPresentation();
  updateVfoTxPresentation();
}

void QtVfoToolRow::buildUi()
{
  m_rowLayout = new QHBoxLayout(this);
  m_rowLayout->setContentsMargins(kToolRowHPadPx, kToolRowVPadPx, kToolRowHPadPx, kToolRowVPadPx);
  m_rowLayout->setSpacing(6);

  m_muteButton = new QToolButton(this);
  m_muteButton->setAutoRaise(true);
  m_muteButton->setFocusPolicy(Qt::NoFocus);
  m_muteButton->setCursor(Qt::PointingHandCursor);
  m_muteButton->setIconSize(QSize(kToolIconSizePx, kToolIconSizePx));
  m_muteButton->setFixedSize(kToolButtonSizePx, kToolButtonSizePx);
  // m_muteButton->setStyleSheet(kButtonStyle);
  connect(m_muteButton, &QToolButton::clicked, this, [this] {
    setMuted(!m_muted);
    emit muteToggledRequested(m_muted);
  });

  m_multiButton = new QToolButton(this);
  m_multiButton->setAutoRaise(true);
  m_multiButton->setFocusPolicy(Qt::NoFocus);
  m_multiButton->setCursor(Qt::PointingHandCursor);
  m_multiButton->setIconSize(QSize(kToolIconSizePx, kToolIconSizePx));
  m_multiButton->setFixedSize(kToolButtonSizePx, kToolButtonSizePx);
  // m_multiButton->setStyleSheet(kButtonStyle);
  connect(m_multiButton, &QToolButton::clicked, this, [this] {
    emit multiVfoActionRequested(m_multiVfoAction);
  });

  m_txSlot = new QWidget(this);
  m_txSlot->setFixedSize(kToolButtonSizePx, kToolButtonSizePx);
  auto* txSlotLayout = new QHBoxLayout(m_txSlot);
  txSlotLayout->setContentsMargins(0, 0, 0, 0);
  txSlotLayout->setSpacing(0);

  m_txButton = new QToolButton(m_txSlot);
  m_txButton->setAutoRaise(true);
  m_txButton->setFocusPolicy(Qt::NoFocus);
  m_txButton->setCursor(Qt::PointingHandCursor);
  m_txButton->setFixedSize(kToolButtonSizePx, kToolButtonSizePx);
  // m_txButton->setStyleSheet(kButtonStyle);
  connect(m_txButton, &QToolButton::clicked, this, [this] {
    emit vfoTxActionRequested(m_vfoTxAction);
  });

  txSlotLayout->addWidget(m_txButton);

  // Layout: left / center / right with stretches to reduce wrong taps.
  m_rowLayout->addWidget(m_muteButton);
  m_rowLayout->addStretch(1);
  m_rowLayout->addWidget(m_multiButton);
  m_rowLayout->addStretch(1);
  m_rowLayout->addWidget(m_txSlot);

  setWidgetProperties(true);
}

void QtVfoToolRow::setEdgePaddingPx(int leftExtraPx, int rightExtraPx)
{
  if (!m_rowLayout) return;

  // Base + per-edge extras
  m_rowLayout->setContentsMargins(
    kToolRowHPadPx + std::max(0, leftExtraPx),
    kToolRowVPadPx,
    kToolRowHPadPx + std::max(0, rightExtraPx),
    kToolRowVPadPx
  );
}

void
QtVfoToolRow::setWidgetProperties(bool repolish)
{
  using P = WidgetProperty;
  static std::array<P, 1> props; // For now

  props[0] = P("role", "vfoToolbar");
  QWidgetPropertySetter::setWidgetProperties(this, props, false);
  QWidgetPropertySetter::setWidgetProperty(m_muteButton, "role", "muteVfo", false);
  QWidgetPropertySetter::setWidgetProperty(m_multiButton, "role", "splitVfo", false);
  QWidgetPropertySetter::setWidgetProperty(m_txButton, "role", "txVfo", false);
  if (repolish) {
    QWidgetPropertySetter::repolishRecursively(this);
  }

}

void QtVfoToolRow::setButtonProperties(QToolButton* button, bool repolish)
{

}

void
QtVfoToolRow::setPttProperty(bool ptt, bool repolish)
{
  QWidgetPropertySetter::setWidgetProperty(this, "ptt", ptt, repolish);
}

void QtVfoToolRow::setMuted(bool muted)
{
  if (m_muted == muted) return;
  m_muted = muted;
  updateMutePresentation();
}

void QtVfoToolRow::updateMutePresentation()
{
  if (!m_muteButton) return;

  // Use standard icons so you don't need to add SVGs yet.
  const QIcon icon = style()->standardIcon(m_muted ? QStyle::SP_MediaVolumeMuted
                                                   : QStyle::SP_MediaVolume);
  m_muteButton->setIcon(icon);
  m_muteButton->setToolTip(m_muted ? "Unmute" : "Mute");
}

bool QtVfoToolRow::isInteractiveChildAt(const QPoint& localPos) const
{
  if (auto* w = childAt(localPos)) {
    if (qobject_cast<QAbstractButton*>(w)) return true;
    if (w->parentWidget() && qobject_cast<QAbstractButton*>(w->parentWidget())) return true;
  }
  return false;
}

void QtVfoToolRow::setMultiVfoAction(MultiVfoAction mode)
{
  if (m_multiVfoAction == mode) return;
  m_multiVfoAction = mode;
  updateMultiVfoPresentation();
}

void QtVfoToolRow::setVfoTxAction(VfoTxAction mode)
{
  if (m_vfoTxAction == mode) return;
  m_vfoTxAction = mode;
  updateVfoTxPresentation();
}

void QtVfoToolRow::updateMultiVfoPresentation()
{
  if (!m_multiButton) return;

  switch (m_multiVfoAction) {
    case MultiVfoAction::None:
      m_multiButton->hide();
      m_multiButton->setIcon(QIcon());
      m_multiButton->setToolTip(QString());
      m_multiButton->setEnabled(false);
      break;

    case MultiVfoAction::Multi:
      m_multiButton->show();
      m_multiButton->setEnabled(true);
      m_multiButton->setIcon(QIcon(":/ui/icons/solid/arrows-left-right-to-line.svg"));
      if (m_multiButton->toolTip().isEmpty()) m_multiButton->setToolTip("Split VFO");
      break;

    case MultiVfoAction::Close:
      m_multiButton->show();
      m_multiButton->setEnabled(true);
      m_multiButton->setIcon(QIcon(":/ui/icons/solid/circle-xmark.svg"));
      if (m_multiButton->toolTip().isEmpty()) m_multiButton->setToolTip("Close split");
      break;
  }
}

void QtVfoToolRow::updateVfoTxPresentation()
{
  if (!m_txButton) return;

  switch (m_vfoTxAction) {
    case VfoTxAction::None:
      m_txButton->hide();
      m_txButton->setIcon(QIcon());
      m_txButton->setToolTip(QString());
      m_txButton->setEnabled(false);
      break;

    case VfoTxAction::Tx:
      m_txButton->show();
      m_txButton->setEnabled(true);
      m_txButton->setText("TX");
      if (m_txButton->toolTip().isEmpty()) m_txButton->setToolTip("Set VFO as TX");
      break;
  }
}