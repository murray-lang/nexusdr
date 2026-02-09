//
// Created by murray on 3/2/26.
//

#pragma once

#include <QWidget>
#include <QStackedLayout>

#include "settings/bands/BandSettings.h"
#include "settings/bands/SplitBandId.h"
#include "VfoReadout/QtVfoReadout.h"
#include "VfoReadout/VfoActions.h"

class QToolButton;

class QtBandReadout final : public QWidget
{
  Q_OBJECT
public:
  enum class BandAction { Split, Tx, Close, Disabled };

  explicit QtBandReadout(SplitBandId bandIndex, QWidget* parent = nullptr);
  ~QtBandReadout() override = default;

  void initialise();

  [[nodiscard]] SplitBandId bandIndex() const noexcept { return m_bandIndex; }

  // VFO cells are now proper standalone widgets
  [[nodiscard]] QtVfoReadout* vfoA() const noexcept { return m_vfoA; }
  [[nodiscard]] QtVfoReadout* vfoB() const noexcept { return m_vfoB; }

  void setIsFocusBand(bool focus, bool repolish = true);
  void setIsTxBand(bool focus, bool repolish = true);
  void setRowVisible(bool visible);

  // Controls what the *left* button means for this band row.
  void setBandActionMode(BandAction mode);

  // Controls whether the right readout area shows VFO B or a placeholder
  void setVfoBVisible(bool visible);

  void setTxButtonVisible(bool visible);

  void applyBandSettings(const BandSettings* bandSettings,
                         const std::string& txBandName,
                         const std::string& rxBandName,
                         const std::string& focusBandName);

  void applyFrequencyChanges(const BandSettings* bandSettings, bool onlyIfChanged = true);

  void setWidgetProperties(bool repolish = true);

  void setPttProperty(bool ptt, bool repolish = true);
  void setIsBandSplitProperty(bool isSplit, bool repolish = true);

signals:
  void splitRequested(SplitBandId whichBand);
  void closeRequested(SplitBandId whichBand);
  void txBandRequested(SplitBandId whichBand);

  void multiVfoActionRequested(SplitBandId whichBand, VfoId whichVfo, MultiVfoAction action);
  void vfoTxActionRequested(SplitBandId whichBand, VfoId whichVfo, VfoTxAction mode);
  void bandClicked(SplitBandId whichBand);
  void vfoClicked(SplitBandId whichBand, VfoId whichVfo);

protected:
  void mousePressEvent(QMouseEvent* e) override;
  bool event(QEvent* e) override;

private slots:
  void onLeftActionClicked();
  void onRightActionClicked();

private:
  void buildUi();
  void updateLeftButtonPresentation();

  static void applyFrequencyChanges(QtVfoReadout* readout,
                                   const class RxPipelineSettings* rxPipelineSettings,
                                   bool onlyIfChanged);

  // static void applyFocus(QtVfoReadout* w, bool focusVfo, bool focusRx, bool focusTx);

  static constexpr int kActionSlotWidthPx = 28;

  SplitBandId m_bandIndex;
  // std::string m_bandName;

  // Layout pieces
  QToolButton* m_addRemoveBandButton = nullptr;
  QToolButton* m_txBandButton = nullptr;

  QtVfoReadout* m_vfoA = nullptr;
  QtVfoReadout* m_vfoB = nullptr;

  QWidget* m_vfoPlaceholder = nullptr;
  QStackedLayout* m_rightStack = nullptr;

  QHBoxLayout* m_rowLayout = nullptr;
  QWidget* m_leftCell = nullptr;
  QWidget* m_rightCell = nullptr;

  QWidget* m_rightStackHost = nullptr;

  BandAction m_addRemoveBandAction = BandAction::Disabled;
  BandAction m_selectTxBandAction = BandAction::Tx;
};