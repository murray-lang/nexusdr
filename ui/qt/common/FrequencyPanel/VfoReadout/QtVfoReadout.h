#pragma once

#include <QWidget>
#include <span>
#include <string>

#include "../../QtNumberReadout.h"
#include "VfoActions.h"
#include "SampleTypes.h"
#include "settings/pipeline/PipelineId.h"

class QtVfoToolRow;

class QtVfoReadout final : public QWidget
{
  Q_OBJECT
public:
  explicit QtVfoReadout(VfoId id, QWidget* parent = nullptr);
  ~QtVfoReadout() override = default;

  // Forwarded API (public surface = “a VFO cell”, not “a number label”)
  void setValue(int64_t value);
  void setValue(sdrreal value);
  void setValueText(const std::string& value);
  void applyFocusAndTx(bool focus, bool tx);

  void setMuted(bool muted);

  void setMultiVfoAction(MultiVfoAction mode);
  [[nodiscard]] MultiVfoAction multiVfoAction() const noexcept { return m_multiVfoAction; }

  void setVfoTxAction(VfoTxAction mode);
  [[nodiscard]] VfoTxAction vfoTxAction() const noexcept { return m_vfoTxAction; }

  void setWidgetProperties(bool repolish = true);
  void setPttProperty(bool ptt, bool repolish = true);

  signals:
    void muteToggledRequested(VfoId id, bool muted);
    void multiVfoActionRequested(MultiVfoAction mode);
    void vfoTxActionRequested(VfoTxAction mode);
    void clicked(VfoId id);

protected:
  void mousePressEvent(QMouseEvent* e) override;
  bool event(QEvent* e) override;

private:
  void buildUi();
  [[nodiscard]] bool isInteractiveChildAt(const QPoint& localPos) const;

  VfoId m_id;
  bool m_hasFocus;
  bool m_isTx;

  QtNumberReadout* m_readout = nullptr;
  QtVfoToolRow* m_toolRow = nullptr;

  MultiVfoAction m_multiVfoAction = MultiVfoAction::None;
  VfoTxAction m_vfoTxAction = VfoTxAction::None;
};