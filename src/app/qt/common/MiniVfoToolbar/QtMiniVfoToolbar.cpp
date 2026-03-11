//
// Created by murray on 11/2/26.
//

#include "QtMiniVfoToolbar.h"

#include <QIcon>
#include <QStyle>
#include <QToolButton>
#include <array>

#include "ui_QtMiniVfoToolbar.h"
#include "core/config-settings/settings/base/SettingUpdateHelpers.h"
#include "app/qt/common/QWidgetPropertySetter.h"

const auto closeIconName = QStringLiteral(":/ui/icons/solid/circle-xmark.svg");
const auto openIconName = QStringLiteral(":/ui/icons/solid/arrow-right.svg");
const auto abButtonText = QStringLiteral("A/B");

QtMiniVfoToolbar::QtMiniVfoToolbar(QWidget* parent)
  : QWidget(parent)
  , ui(std::make_unique<Ui::QtMiniVfoToolbar>())
{
  setAttribute(Qt::WA_StyledBackground, true);
  setAttribute(Qt::WA_AcceptTouchEvents, true);

  ui->setupUi(this);

  ui->muteButton->setFocusPolicy(Qt::NoFocus);
  ui->abButton->setFocusPolicy(Qt::NoFocus);
  ui->txButton->setFocusPolicy(Qt::NoFocus);
  ui->modeButton->setFocusPolicy(Qt::NoFocus);

  connect(ui->muteButton, &QToolButton::clicked, this, [this] {
    setMuted(!m_muted);
    emit muteToggledRequested(m_muted);
  });

  connect(ui->abButton, &QToolButton::clicked, this, [this] { onAbPressed(); });
  connect(ui->txButton, &QToolButton::clicked, this, [this] { onTxPressed(); });
  connect(ui->modeButton, &QToolButton::clicked, this, [this] { onModePressed(); });

  // lockAbButtonWidthToIconState();
  setFixedProperties();
  updateMutePresentation();
  updateAbPresentation();
  setTxActive(false);
}

QtMiniVfoToolbar::~QtMiniVfoToolbar() = default;

void QtMiniVfoToolbar::setContext(SplitBandId bandId, VfoId vfoId)
{
  m_bandId = bandId;
  m_vfoId = vfoId;
  // refreshFromSettings();
}

void QtMiniVfoToolbar::setFixedProperties()
{
  QWidgetPropertySetter::setWidgetProperty(ui->muteButton, "role", "mute", false);
  QWidgetPropertySetter::setWidgetProperty(ui->abButton, "role", "ab", false);
  QWidgetPropertySetter::setWidgetProperty(ui->txButton, "role", "tx", false);
  QWidgetPropertySetter::setWidgetProperty(ui->modeButton, "role", "mode", false);
}

void QtMiniVfoToolbar::applyBandSettings(const BandSettings* bandSettings, bool isFocusBand, bool isTxBand)
{
  m_bandSettings = bandSettings;
  refreshFromSettings(isFocusBand, isTxBand);
}

void QtMiniVfoToolbar::setMuted(bool muted)
{
  if (m_muted == muted) return;
  m_muted = muted;
  updateMutePresentation();
}

void QtMiniVfoToolbar::setAbActive(bool active)
{
  if (m_abActive == active) return;
  m_abActive = active;
  updateAbPresentation();
}

void QtMiniVfoToolbar::setTxActive(bool active)
{
  ui->txButton->setProperty("active", active);
  ui->txButton->style()->unpolish(ui->txButton);
  ui->txButton->style()->polish(ui->txButton);
  ui->txButton->update();
}

void QtMiniVfoToolbar::setModeText(const QString& text)
{
  ui->modeButton->setText(text.isEmpty() ? QStringLiteral("Mode") : text);
}

void QtMiniVfoToolbar::updateMutePresentation()
{
  ui->muteButton->setProperty("active", m_muted);
  ui->muteButton->setText(QStringLiteral("Mute"));
  ui->muteButton->style()->unpolish(ui->muteButton);
  ui->muteButton->style()->polish(ui->muteButton);
  ui->muteButton->update();
}

void QtMiniVfoToolbar::updateAbPresentation()
{
  ui->abButton->setText(abButtonText);
  ui->abButton->setProperty("active", m_abActive);

  ui->abButton->setIcon(m_abActive
    ? QIcon(closeIconName)
    : QIcon(openIconName));

  ui->abButton->style()->unpolish(ui->abButton);
  ui->abButton->style()->polish(ui->abButton);
  ui->abButton->update();
}

bool QtMiniVfoToolbar::hasValidContext() const noexcept
{
  return m_bandId != SplitBandId::None;
}

void QtMiniVfoToolbar::refreshFromSettings(bool isFocusBand, bool isTxBand)
{
  if (m_bandSettings == nullptr) {
    setAbActive(false);
    setTxActive(false);
    return;
  }

  setAbActive(m_bandSettings->isMultiPipeline());
  setTxActive(m_bandSettings->getTxPipelineId() == m_vfoId && isTxBand);
}

void QtMiniVfoToolbar::onAbPressed()
{
  if (!hasValidContext() || m_bandSettings == nullptr) return;

  ;
  if (!m_bandSettings->isMultiPipeline()) {
    QVector<SettingUpdate> update = {{ SettingUpdateHelpers::makeSetMultiPipeline(m_bandId, true) }};
    emit settingUpdateRequested(update);
    return;
  }

  // Close the pipeline associated with *this* VFO cell (A can close A; B can close B).
  QVector<SettingUpdate> update = {{ SettingUpdateHelpers::makeClosePipeline(m_bandId, m_vfoId)}};
  emit settingUpdateRequested(update);
}

void QtMiniVfoToolbar::onTxPressed()
{
  if (!hasValidContext()) return;

  QVector<SettingUpdate> update = {
    {
      SettingUpdateHelpers::makeSetTxBand(m_bandId),
      SettingUpdateHelpers::makeSetTxPipeline(m_bandId, m_vfoId)
    }};
  emit settingUpdateRequested(update);
}

void QtMiniVfoToolbar::onModePressed()
{
  if (!hasValidContext()) return;

  // The selection UI (menu/popup) likely lives above this widget.
  // When a mode is chosen, that code should call:
  //   settingUpdateRequested(RadioSettings::makeBandSetModeOnFocusPipelineUpdate(m_bandId, modeType));
  emit modeSelectRequested(m_bandId);
}

void QtMiniVfoToolbar::lockAbButtonWidthToIconState()
{
  if (!ui || !ui->abButton) return;

  const QIcon closeIcon(closeIconName);

  const QIcon oldIcon = ui->abButton->icon();
  const QString oldText = ui->abButton->text();

  ui->abButton->setText(oldText.isEmpty() ? abButtonText : oldText);
  ui->abButton->setIcon(closeIcon);

  // Ensure any QSS-driven metrics are applied before measuring.
  ui->abButton->ensurePolished();

  const int w = ui->abButton->sizeHint().width();
  if (w > 0) {
    ui->abButton->setMinimumWidth(w);
    ui->abButton->setMaximumWidth(w);
  }

  ui->abButton->setIcon(oldIcon);
  ui->abButton->setText(oldText);
}