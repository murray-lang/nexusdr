//
// Created by murray on 9/2/26.
//

#pragma once

#include <QWidget>
#include <QPoint>

#include "VfoActions.h"

class QHBoxLayout;
class QToolButton;

class QtVfoToolRow final : public QWidget
{
  Q_OBJECT
public:
  explicit QtVfoToolRow(QWidget* parent = nullptr);
  ~QtVfoToolRow() override = default;

  void setMuted(bool muted);
  [[nodiscard]] bool isMuted() const noexcept { return m_muted; }

  void setMultiVfoAction(MultiVfoAction mode);
  [[nodiscard]] MultiVfoAction multiVfoAction() const noexcept { return m_multiVfoAction; }

  void setVfoTxAction(VfoTxAction mode);
  [[nodiscard]] VfoTxAction vfoTxAction() const noexcept { return m_vfoTxAction; }

  void setEdgePaddingPx(int leftExtraPx, int rightExtraPx);

  [[nodiscard]] bool isInteractiveChildAt(const QPoint& localPos) const;

  void setWidgetProperties(bool repolish = true);
  void setButtonProperties(QToolButton* button, bool repolish = true);
  void setPttProperty(bool ptt, bool repolish = true);

  signals:
    void muteToggledRequested(bool muted);
    void multiVfoActionRequested(MultiVfoAction mode);
    void vfoTxActionRequested(VfoTxAction mode);

private:
  void buildUi();
  void updateMutePresentation();
  void updateMultiVfoPresentation();
  void updateVfoTxPresentation();

  QHBoxLayout* m_rowLayout = nullptr;

  static constexpr int kToolButtonSizePx = 20; // hit target
  static constexpr int kToolIconSizePx = 16;   // visual icon
  static constexpr int kToolRowHPadPx = 4;
  static constexpr int kToolRowVPadPx = 2;

  QToolButton* m_muteButton = nullptr;
  QToolButton* m_multiButton = nullptr;

  QWidget* m_txSlot = nullptr;
  QToolButton* m_txButton = nullptr;

  bool m_muted = false;
  MultiVfoAction m_multiVfoAction = MultiVfoAction::None;
  VfoTxAction m_vfoTxAction = VfoTxAction::None;
};