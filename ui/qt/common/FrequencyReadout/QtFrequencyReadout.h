//
// Created by murray on 28/1/26.
//

#pragma once
#include <QObject>
#include <QWidget>
#include <memory>

#include "settings/bands/BandSelector.h"
#include "settings/base/SettingUpdateSink.h"
#include "settings/base/SettingUpdateSource.h"

class RadioSettings;
class QtNumberReadout;
class QStackedLayout;
class QToolButton;

namespace Ui { class QtFrequencyReadout; }

class QtFrequencyReadout : public QWidget, public SettingUpdateSource
{
  Q_OBJECT
public:
  explicit QtFrequencyReadout(QWidget* parent = nullptr);
  ~QtFrequencyReadout() override;

  void initialise(RadioSettings* pRadioSettings);
  void applyRadioSettings(RadioSettings* pRadioSettings, bool onlyIfChanged = true);

  void connectSettingUpdateSink(SettingUpdateSink* pSink) override { m_pSettingsSink = pSink; };

protected:
  void notifySettingUpdate(SettingUpdate& settingUpdate) override
  {
    if (m_pSettingsSink != nullptr) {
      m_pSettingsSink->applySettingUpdate(settingUpdate);
    }
  }

private slots:
  void onSplitClicked();
  void onCloseBandA();
  void onCloseBandB();

private:
  struct RowWidgets
  {
    QWidget* row = nullptr;              // styling container
    QWidget* leftCell = nullptr;         // contains (action button + VFO A)
    QWidget* rightCell = nullptr;        // contains stack
    QStackedLayout* rightStack = nullptr;
    QWidget* rightPlaceholder = nullptr;
    QToolButton* actionButton = nullptr; // Split or Close depending on state
  };

  RowWidgets createBandRow(const QString& rowObjectName,
                      QtNumberReadout* vfoA,
                      QtNumberReadout* vfoB);

  void initialiseGridLayout();
  void applyFrequencyChanges(BandSelector& bandSelector, bool onlyIfChanged = true);
  void applyFrequencyChanges(
    QtNumberReadout* readout,
    const RxPipelineSettings* rxPipelineSettings,
    bool onlyIfChanged = true
  );
  void applyBandSelectorChange(BandSelector& bandSelector);
  void applyBandSettings(
    QtNumberReadout* pVfoReadoutA,
    QtNumberReadout* pVfoReadoutB,
    const BandSettings* bandSettings,
    const std::string& txBandName,
    const std::string& rxBandName,
    const std::string& focusBandName,
    bool* pShowVfoA,
    bool* pShowVfoB
  );
  static void applyStyleContext(QtNumberReadout* w,
                             const char* vfo,      // "A" or "B"
                             const char* band,     // "A" or "B"
                             bool focusVfo,
                             bool focusRx,
                             bool focusTx,
                             bool ptt);

  void applyPtt(bool ptt);
  static void applyPtt(QtNumberReadout* w, bool ptt);
  static void applyFocus(QtNumberReadout* w, bool focusVfo, bool focusRx, bool focusTx);

  void showHide(bool show_bandA_vfoA, bool show_bandA_vfoB, bool show_bandB_vfoA, bool show_bandB_vfoB);

  void applyRowStretch(
    QWidget* row,
    QWidget* left,
    bool showLeft,
    QWidget* right,
    bool showRight
    ) const;

  void updateRowActionButtons(bool showBandARow, bool showBandBRow, bool isSplit);

  std::unique_ptr<Ui::QtFrequencyReadout> ui;

  SettingUpdateSink* m_pSettingsSink;

  QtNumberReadout* m_pReadoutBandA_VfoA;
  QtNumberReadout* m_pReadoutBandA_VfoB;
  QtNumberReadout* m_pReadoutBandB_VfoA;
  QtNumberReadout* m_pReadoutBandB_VfoB;

  QWidget* m_pBandARow;
  QWidget* m_pBandBRow;

  QWidget* m_pBandA_LeftCell;
  QWidget* m_pBandA_RightCell;
  QWidget* m_pBandB_LeftCell;
  QWidget* m_pBandB_RightCell;

  QStackedLayout* m_pBandA_RightStack;
  QStackedLayout* m_pBandB_RightStack;
  QWidget* m_pBandA_RightPlaceholder;
  QWidget* m_pBandB_RightPlaceholder;

  QToolButton* m_pBandA_ActionButton;
  QToolButton* m_pBandB_ActionButton;
};