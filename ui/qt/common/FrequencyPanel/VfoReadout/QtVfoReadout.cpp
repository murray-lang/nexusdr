#include "QtVfoReadout.h"

#include <QEvent>
#include <QMouseEvent>
#include <QVBoxLayout>

#include "../../QtNumberReadout.h"
#include "QtVfoToolRow.h"

QtVfoReadout::QtVfoReadout(VfoId id, QWidget* parent)
  : QWidget(parent)
  , m_id(id)
  , m_hasFocus(true)
  , m_isTx(true)
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

  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(0, 0, 0, 0);
  root->setSpacing(0);
  root->addWidget(m_readout, /*stretch*/ 1);

  m_toolRow = new QtVfoToolRow(this);

  connect(m_toolRow, &QtVfoToolRow::muteToggledRequested, this, [this](bool muted) {
    emit muteToggledRequested(m_id, muted);
  });
  connect(m_toolRow, &QtVfoToolRow::multiVfoActionRequested,
          this, &QtVfoReadout::multiVfoActionRequested);
  connect(m_toolRow, &QtVfoToolRow::vfoTxActionRequested,
          this, &QtVfoReadout::vfoTxActionRequested);

  static constexpr int kInterVfoToolGutterPx = 5;
  if (m_id == VfoId::A) {
    m_toolRow->setEdgePaddingPx(0, kInterVfoToolGutterPx);
  } else {
    m_toolRow->setEdgePaddingPx(kInterVfoToolGutterPx, 0);
  }

  root->addWidget(m_toolRow, /*stretch*/ 0);

  // Keep UI in sync with state (setMultiVfoAction/setVfoTxAction will also do this later)
  m_toolRow->setMultiVfoAction(m_multiVfoAction);
  m_toolRow->setVfoTxAction(m_vfoTxAction);

  setWidgetProperties(true);
}

void QtVfoReadout::setMuted(bool muted)
{
  if (m_toolRow) m_toolRow->setMuted(muted);
}

bool QtVfoReadout::isInteractiveChildAt(const QPoint& localPos) const
{
  if (!m_toolRow) return false;

  const QPoint inToolRow = m_toolRow->mapFrom(this, localPos);
  if (!m_toolRow->rect().contains(inToolRow)) return false;

  return m_toolRow->isInteractiveChildAt(inToolRow);
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

// ... existing code ...

void QtVfoReadout::setMultiVfoAction(MultiVfoAction mode)
{
  if (m_multiVfoAction == mode) return;
  m_multiVfoAction = mode;
  if (m_toolRow) m_toolRow->setMultiVfoAction(mode);
}

void QtVfoReadout::setVfoTxAction(VfoTxAction mode)
{
  if (m_vfoTxAction == mode) return;
  m_vfoTxAction = mode;
  if (m_toolRow) m_toolRow->setVfoTxAction(mode);
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

void QtVfoReadout::mousePressEvent(QMouseEvent* e)
{
  if (!e) return;

  if (e->button() == Qt::LeftButton) {
    if (isInteractiveChildAt(e->position().toPoint())) {
      QWidget::mousePressEvent(e);
      return;
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

  if (e->type() == QEvent::TouchEnd) {
    auto* te = dynamic_cast<QTouchEvent*>(e);
    const auto& points = te->points();
    if (!points.isEmpty()) {
      const QPoint pos = points.first().position().toPoint();
      if (rect().contains(pos)) {
        if (isInteractiveChildAt(pos)) {
          return QWidget::event(e);
        }

        emit clicked(m_id);
        e->accept();
        return true;
      }
    }
  }

  return QWidget::event(e);
}