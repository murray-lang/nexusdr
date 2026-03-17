#include "QtVfoReadout.h"

#include <QEvent>
#include <QGraphicsWidget>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QStyleOption>

#include "../../QtNumberReadout.h"
#include "../../MiniVfoToolbar/QtMiniVfoToolbar.h"

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

void QtVfoReadout::setBandId(SplitBandId bandId)
{
  m_splitBandId = bandId;
  if (m_toolbar) {
    m_toolbar->setContext(m_splitBandId, m_id);
  }
}

void
QtVfoReadout::applyBandSettings(const BandSettings* bandSettings, bool isLoneVfo, bool isFocusBand, bool isTxBand)
{
  if (isLoneVfo) {
    setMultiVfoAction(MultiVfoAction::Multi);
    setVfoTxAction(VfoTxAction::None);
  } else {
    setMultiVfoAction(MultiVfoAction::Close);
    setVfoTxAction(isTxBand ? VfoTxAction::Tx : VfoTxAction::None);
  }

  const VfoId focusVfoId = bandSettings->getFocusPipelineId();
  const VfoId txVfoId = bandSettings->getTxPipelineId();
  applyFocusAndTx(focusVfoId == m_id && isFocusBand, txVfoId == m_id && isTxBand);

  if (m_toolbar) {
    m_toolbar->applyBandSettings(bandSettings, isFocusBand, isTxBand);
  }
}

void QtVfoReadout::setModeText(const QString& text)
{
  if (m_toolbar) {
    m_toolbar->setModeText(text);
  }
}

void QtVfoReadout::buildUi()
{
  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(0, 0, 0, 0);
  root->setSpacing(0);

  // Mini toolbar above the digits
  m_toolbar = new QtMiniVfoToolbar(this);

  // Ensure the toolbar knows which band/VFO it belongs to (may be None initially).
  m_toolbar->setContext(m_splitBandId, m_id);

  connect(m_toolbar, &QtMiniVfoToolbar::muteToggledRequested, this, [this](bool muted) {
    emit muteToggledRequested(m_id, muted);
  });

  // Forward setting updates upward (panel/face will apply them)
  connect(m_toolbar, &QtMiniVfoToolbar::settingUpdateRequested,
          this, &QtVfoReadout::settingUpdateRequested);

  // If you want the parent to open a mode chooser UI:
  connect(m_toolbar, &QtMiniVfoToolbar::modeSelectRequested, this, [this](SplitBandId /*bandId*/) {
    // The toolbar needs context (bandId) to be meaningful; you can choose to ignore here
    // and let a higher layer handle mode selection based on current focus.
  });

  root->addWidget(m_toolbar, /*stretch*/ 0, Qt::AlignRight);

  // Digits
  m_readout = new QtNumberReadout(this);
  root->addWidget(m_readout, /*stretch*/ 0);

  // if (m_toolbar) {
  //   m_toolbar->setAbActive(m_multiVfoAction == MultiVfoAction::Close);
  //   m_toolbar->setTxActive(m_vfoTxAction == VfoTxAction::Tx);
  // }

  root->addStretch(1);

  setWidgetProperties(true);

  if (m_readout) m_readout->installEventFilter(this);
  if (m_readout && m_readout->valueLabel()) m_readout->valueLabel()->installEventFilter(this);

  scheduleWidthRecompute();
}

void QtVfoReadout::updateFixedWidth()
{
  bool ok = false;
  const int forced = property("vfoWidthPx").toInt(&ok);
  if (ok && forced > 0) {
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setMinimumWidth(forced);
    setMaximumWidth(forced);
    return;
  }

  int readoutW = 0;

  if (m_readout && m_readout->valueLabel()) {
    QLabel* label = m_readout->valueLabel();

    // QString sample = property("vfoWidthSample").toString();
    // if (sample.isEmpty()) sample = QStringLiteral("0000.000000");

    const QFontMetrics fm(label->font());
    const int textW = fm.horizontalAdvance("0000.000000");

    const QMargins cm = label->contentsMargins();
    const int labelMargin = label->margin();     // QLabel::margin()
    const int frameW = label->frameWidth();      // QLabel inherits QFrame; QSS borders affect this

    // This is the common set of "invisible" pixels that cause that last digit to clip.
    const int chromeW =
      cm.left() + cm.right() +
      2 * labelMargin +
      2 * frameW;

    // If QtNumberReadout ever gets margins, include them too.
    const QMargins rm = m_readout->contentsMargins();

    // Safety pad for rounding / style quirks.
    // Bump this if you still see clipping with certain fonts.
    static constexpr int kSafetyPadPx = 12;

    readoutW = textW + chromeW + rm.left() + rm.right() + kSafetyPadPx;
  }

  const int toolsW = m_toolbar ? m_toolbar->sizeHint().width() : 0;

  const int w = std::max(readoutW, toolsW);
  if (w <= 0) return;

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  setMinimumWidth(w);
  setMaximumWidth(w);
}

void QtVfoReadout::scheduleWidthRecompute()
{
  QTimer::singleShot(0, this, [this] { updateFixedWidth(); });
}

bool QtVfoReadout::eventFilter(QObject* watched, QEvent* e)
{
  if (!e) return QWidget::eventFilter(watched, e);

  switch (e->type()) {
  case QEvent::Polish:
  case QEvent::PolishRequest:
  case QEvent::StyleChange:
  case QEvent::FontChange:
  case QEvent::DynamicPropertyChange:
    scheduleWidthRecompute();
    break;
  default:
    break;
  }

  return QWidget::eventFilter(watched, e);
}

bool QtVfoReadout::event(QEvent* e)
{
  if (!e) return QWidget::event(e);

  const auto recomputeLater = [this]() {
    // Delay one tick so QSS-driven font changes have landed on child widgets.
    QTimer::singleShot(0, this, [this] { updateFixedWidth(); });
  };

  if (e->type() == QEvent::Polish ||
      e->type() == QEvent::PolishRequest ||
      e->type() == QEvent::StyleChange ||
      e->type() == QEvent::FontChange ||
      e->type() == QEvent::DynamicPropertyChange) {
    const bool r = QWidget::event(e);
    recomputeLater();
    return r;
  }

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

void QtVfoReadout::setMuted(bool muted)
{
  if (m_toolbar) m_toolbar->setMuted(muted);
}

bool QtVfoReadout::isInteractiveChildAt(const QPoint& localPos) const
{
  // Toolbar hit testing: treat interactions within its bounds as "interactive"
  // so VFO background taps still work.
  if (m_toolbar) {
    const QPoint inToolbar = m_toolbar->mapFrom(this, localPos);
    if (m_toolbar->rect().contains(inToolbar)) {
      if (m_toolbar->childAt(inToolbar) != nullptr) return true;
    }
  }

  return false;
}

void QtVfoReadout::setWidgetProperties(bool repolish)
{
  using P = WidgetProperty;
  static std::array<P, 4> props;

  props[0] = P("role", "vfoReadout");
  props[1] = P("vfo", m_id == VfoId::A ? "A" : "B");
  props[2] = P("focusVfo", m_hasFocus);
  props[3] = P("txVfo", m_isTx);

  QWidgetPropertySetter::setWidgetProperties(this, props, repolish);
}

void QtVfoReadout::setPttProperty(bool ptt, bool repolish)
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

  if (m_toolbar) {
    // "A/B" is lit and shows close glyph when multi is active
    m_toolbar->setAbActive(mode == MultiVfoAction::Close);
  }
}

void QtVfoReadout::setVfoTxAction(VfoTxAction mode)
{
  if (m_vfoTxAction == mode) return;
  m_vfoTxAction = mode;

  if (m_toolbar) {
    m_toolbar->setTxActive(mode == VfoTxAction::Tx);
  }
}

void QtVfoReadout::applyFocusAndTx(bool focus, bool tx)
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