#pragma once

#include <QWidget>
#include <span>
#include <string>

#include "../QtNumberReadout.h"   // for StyleProperty / StyleValue + value API
#include "SampleTypes.h"
#include "settings/pipeline/PipelineId.h"

class QToolButton;

class QtVfoReadout final : public QWidget
{
  Q_OBJECT
public:
  enum class MultiVfoAction { None, Multi, Close };
  enum class VfoTxAction { None, Tx };

  explicit QtVfoReadout(VfoId id, QWidget* parent = nullptr);
  ~QtVfoReadout() override = default;

  // Forwarded API (public surface = “a VFO cell”, not “a number label”)
  void setValue(int64_t value);
  void setValue(sdrreal value);
  void setValueText(const std::string& value);
  // void applyStyleProperties(std::span<const StyleProperty> props);
  void applyFocusAndTx(bool focus, bool tx);

  void setMultiVfoAction(MultiVfoAction mode);
  [[nodiscard]] MultiVfoAction multiVfoAction() const noexcept { return m_multiVfoAction; }

  void setVfoTxAction(VfoTxAction mode);
  [[nodiscard]] VfoTxAction vfoTxAction() const noexcept { return m_vfoTxAction; }

  void setWidgetProperties(bool repolish = true);
  void setPttProperty(bool ptt, bool repolish = true);

  signals:
    void multiVfoActionRequested(MultiVfoAction mode);
    void vfoTxActionRequested(VfoTxAction mode);
    void clicked(VfoId id);

protected:
  void resizeEvent(QResizeEvent* e) override;
  void showEvent(QShowEvent* e) override;

  void mousePressEvent(QMouseEvent* e) override;
  bool event(QEvent* e) override;

private:
  void buildUi();
  void updateMultiVfoPresentation();
  void updateVfoTxPresentation();
  void layoutMultiButton();
  void layoutTxButton();

  static constexpr int kBadgeSizePx = 24;
  static constexpr int kBadgeInsetPx = 2;

  VfoId m_id;
  bool m_hasFocus;
  bool m_isTx;
  QtNumberReadout* m_readout = nullptr;
  QToolButton* m_multiButton = nullptr;
  QToolButton* m_txButton = nullptr;
  MultiVfoAction m_multiVfoAction = MultiVfoAction::None;
  VfoTxAction m_vfoTxAction = VfoTxAction::None;
};