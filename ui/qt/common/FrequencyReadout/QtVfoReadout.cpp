#include "QtVfoReadout.h"

#include <QIcon>
#include <QResizeEvent>
#include <QToolButton>
#include <QVBoxLayout>
#include <QTimer>

#include "../QtNumberReadout.h"

QtVfoReadout::QtVfoReadout(VfoId id, QWidget* parent)
  : QWidget(parent)
  , m_id(id)
{
  setAttribute(Qt::WA_StyledBackground, true);
  setAttribute(Qt::WA_AcceptTouchEvents, true);
  buildUi();
  setMultiVfoAction(MultiVfoAction::None);
  setVfoTxAction(VfoTxAction::Tx);
}

void QtVfoReadout::buildUi()
{
  m_readout = new QtNumberReadout(this);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(m_readout);

  m_multiButton = new QToolButton(this);
  m_multiButton->setAutoRaise(true);
  m_multiButton->setFocusPolicy(Qt::NoFocus);
  m_multiButton->setCursor(Qt::PointingHandCursor);
  m_multiButton->setIconSize(QSize(kBadgeSizePx - 8, kBadgeSizePx - 8));
  m_multiButton->setFixedSize(kBadgeSizePx, kBadgeSizePx);

  m_multiButton->setStyleSheet(
    "QToolButton {"
    "  border: 1px solid rgba(255,255,255,80);"
    "  border-radius: 12px;"
    "  background: rgba(0,0,0,110);"
    "  padding: 0px;"
    "}"
    "QToolButton:pressed {"
    "  background: rgba(255,255,255,40);"
    "}"
  );
  connect(m_multiButton, &QToolButton::clicked, this, [this] {
    emit multiVfoActionRequested(m_multiVfoAction);
  });

  updateMultiVfoPresentation();
  layoutMultiButton();

  m_txButton = new QToolButton(this);
  m_txButton->setAutoRaise(true);
  m_txButton->setFocusPolicy(Qt::NoFocus);
  m_txButton->setCursor(Qt::PointingHandCursor);
  // m_txButton->setIconSize(QSize(kBadgeSizePx - 8, kBadgeSizePx - 8));
  m_txButton->setFixedSize(kBadgeSizePx, kBadgeSizePx);

  m_txButton->setStyleSheet(
    "QToolButton {"
    "  border: 1px solid rgba(255,255,255,80);"
    "  border-radius: 12px;"
    "  background: rgba(0,0,0,110);"
    "  padding: 0px;"
    "}"
    "QToolButton:pressed {"
    "  background: rgba(255,255,255,40);"
    "}"
  );

  connect(m_txButton, &QToolButton::clicked, this, [this] {
    emit vfoTxActionRequested(m_vfoTxAction);
  });
  updateVfoTxPresentation();
  layoutTxButton();
}

void QtVfoReadout::setValue(int64_t value)
{
  if (m_readout) m_readout->setValue(value);
}

void QtVfoReadout::setValue(sdrreal value)
{
  if (m_readout) m_readout->setValue(value);
}

void QtVfoReadout::setValueText(const std::string& value)
{
  if (m_readout) m_readout->setValueText(value);
}

void
QtVfoReadout::setWidgetProperties(bool repolish)
{
  using P = WidgetProperty;
  static std::array<P, 4> props;

  props[0] = P("role", "vfoReadout");
  props[1] = P("vfo", m_id == VfoId::A ? "A" : "B");
  props[2] = P("focusVfo", m_hasFocus);
  props[3] = P("txVfo", m_isTx);

  QWidgetPropertySetter::setWidgetProperties(this, props, repolish);
}

void
QtVfoReadout::setPttProperty(bool ptt, bool repolish)
{
  QWidgetPropertySetter::setWidgetProperty(this, "ptt", ptt, repolish);
}

void QtVfoReadout::setMultiVfoAction(MultiVfoAction mode)
{
  if (m_multiVfoAction == mode) return;
  m_multiVfoAction = mode;
  updateMultiVfoPresentation();
  layoutMultiButton();
}

void QtVfoReadout::setVfoTxAction(VfoTxAction mode)
{
  if (m_vfoTxAction == mode) return;
  m_vfoTxAction = mode;
  updateVfoTxPresentation();
  layoutTxButton();
}

void
QtVfoReadout::applyFocusAndTx(bool focus, bool tx)
{
  m_hasFocus = focus;
  m_isTx = tx;

  using P = WidgetProperty;
  static std::array<P, 2> props;

  props[0] = P("focusVfo", m_hasFocus);
  props[1] = P("txVfo", m_isTx);

  QWidgetPropertySetter::setWidgetProperties(this, props);
}

void QtVfoReadout::updateMultiVfoPresentation()
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

void QtVfoReadout::updateVfoTxPresentation()
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

void QtVfoReadout::resizeEvent(QResizeEvent* e)
{
  QWidget::resizeEvent(e);
  layoutMultiButton();
}

void QtVfoReadout::showEvent(QShowEvent* e)
{
  QWidget::showEvent(e);

  // On first show, layouts might not have finalized sizes yet. Defer one tick.
  QTimer::singleShot(0, this, [this] {
    layoutMultiButton();
    layoutTxButton();
  });
}

void QtVfoReadout::layoutMultiButton()
{
  if (!m_multiButton || !m_multiButton->isVisible()) return;

  // If we're called early (e.g. during construction/polish), width/height can be 0.
  // Clamping avoids weird negative positions; the deferred showEvent will correct it.
  const int w = width();
  const int h = height();
  if (w <= 0 || h <= 0) return;

  // int x = w - kBadgeInsetPx - m_multiButton->width();
  int x = kBadgeInsetPx;
  int y = kBadgeInsetPx;

  // x = std::clamp(x, 0, w - m_multiButton->width());
  x = std::clamp(x, 0, kBadgeInsetPx);
  y = std::clamp(y, 0, h - m_multiButton->height());

  m_multiButton->move(x, y);
  m_multiButton->raise();
}

void QtVfoReadout::layoutTxButton()
{
  if (!m_txButton || !m_txButton->isVisible()) return;

  // If we're called early (e.g. during construction/polish), width/height can be 0.
  // Clamping avoids weird negative positions; the deferred showEvent will correct it.
  const int w = width();
  const int h = height();
  if (w <= 0 || h <= 0) return;

  int x = kBadgeInsetPx + m_multiButton->width() + kBadgeInsetPx;
  int y = kBadgeInsetPx;

  // x = std::clamp(x, 0, w - m_multiButton->width());
  // x = std::clamp(x, 0, m_multiButton->width());
  y = std::clamp(y, 0, h - m_txButton->height());

  m_txButton->move(x, y);
  m_txButton->raise();
}

void
QtVfoReadout::mousePressEvent(QMouseEvent* e)
{
  if (!e) return;

  if (e->button() == Qt::LeftButton) {
    // If the badge is under the pointer, let the QToolButton handle it.
    if (m_multiButton && m_multiButton->isVisible()) {
      const QPoint badgeLocal = m_multiButton->mapFrom(this, e->position().toPoint());
      if (m_multiButton->rect().contains(badgeLocal)) {
        QWidget::mousePressEvent(e);
        return;
      }
    }
    if (m_txButton && m_txButton->isVisible()) {
      const QPoint badgeLocal = m_txButton->mapFrom(this, e->position().toPoint());
      if (m_txButton->rect().contains(badgeLocal)) {
        QWidget::mousePressEvent(e);
        return;
      }
    }

    emit clicked(m_id);
    e->accept();
    return;
  }

  QWidget::mousePressEvent(e);
}

bool QtVfoReadout::event(QEvent* e)
{
  if (!e) return QWidget::event(e);

  // Basic touch-to-click: treat TouchEnd as a click if it ends inside us,
  // and not on the badge button.
  if (e->type() == QEvent::TouchEnd) {
    auto* te = dynamic_cast<QTouchEvent*>(e);
    const auto& points = te->points();
    if (!points.isEmpty()) {
      const QPointF pos = points.first().position();
      if (rect().contains(pos.toPoint())) {
        if (m_multiButton && m_multiButton->isVisible()) {
          const QPoint badgeLocal = m_multiButton->mapFrom(this, pos.toPoint());
          if (m_multiButton->rect().contains(badgeLocal)) {
            return QWidget::event(e);
          }
        }
        if (m_txButton && m_txButton->isVisible()) {
          const QPoint badgeLocal = m_txButton->mapFrom(this, pos.toPoint());
          if (m_txButton->rect().contains(badgeLocal)) {
            return QWidget::event(e);
          }
        }

        emit clicked(m_id);
        e->accept();
        return true;
      }
    }
  }

  return QWidget::event(e);
}
