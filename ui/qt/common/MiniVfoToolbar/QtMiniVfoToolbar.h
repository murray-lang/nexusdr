//
// Created by murray on 11/2/26.
//

#pragma once

#include <QWidget>
#include <memory>

#include "config-settings/settings/RadioSettings.h"
#include "config-settings/settings/bands/BandSettings.h"
#include "config-settings/settings/bands/SplitBandId.h"
#include "config-settings/settings/base/SettingUpdate.h"
#include "config-settings/settings/pipeline/PipelineId.h"
#include "config-settings/settings/Mode.h"

namespace Ui { class QtMiniVfoToolbar; }

class QtMiniVfoToolbar final : public QWidget
{
  Q_OBJECT
public:
  explicit QtMiniVfoToolbar(QWidget* parent = nullptr);
  ~QtMiniVfoToolbar() override;

  void setContext(SplitBandId bandId, VfoId vfoId);
  void applyBandSettings(const BandSettings* bandSettings, bool isFocusBand, bool isTxBand);

  void setMuted(bool muted);
  [[nodiscard]] bool isMuted() const noexcept { return m_muted; }

  void setAbActive(bool active);          // dual pipeline active? (presentation override)
  [[nodiscard]] bool isAbActive() const noexcept { return m_abActive; }

  void setTxActive(bool active);          // purely presentation (optional)
  void setModeText(const QString& text);  // e.g. "USB"

  signals:
    void muteToggledRequested(bool muted);
    void settingUpdateRequested(QVector<SettingUpdate>& updates);
    // Mode button still needs a chooser UI somewhere; emit this to open it.
    void modeSelectRequested(SplitBandId bandId);

private:
  void setFixedProperties();
  void updateMutePresentation();
  void updateAbPresentation();
  void refreshFromSettings(bool isFocusBand, bool isTxBand);

  void onAbPressed();
  void onTxPressed();
  void onModePressed();

  [[nodiscard]] bool hasValidContext() const noexcept;
  void lockAbButtonWidthToIconState();

  std::unique_ptr<Ui::QtMiniVfoToolbar> ui;

  const BandSettings* m_bandSettings = nullptr;
  SplitBandId m_bandId = SplitBandId::None;
  VfoId m_vfoId = VfoId::A;

  bool m_muted = false;
  bool m_abActive = false;
};