//
// Created by murray on 28/1/26.
//

#pragma once
#include <QObject>
#include <QWidget>
#include <memory>

#include "settings/bands/BandSelector.h"

class RadioSettings;
class QtNumberReadout;

namespace Ui { class QtFrequencyReadout; }

class QtFrequencyReadout : public QWidget
{
  Q_OBJECT
public:
  explicit QtFrequencyReadout(QWidget* parent = nullptr);
  ~QtFrequencyReadout() override;

  void initialise(RadioSettings* pRadioSettings);
  void applyRadioSettings(RadioSettings* pRadioSettings, bool onlyIfChanged = true);

private:
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

  std::unique_ptr<Ui::QtFrequencyReadout> ui;

  QtNumberReadout* m_pReadoutBandA_VfoA;
  QtNumberReadout* m_pReadoutBandA_VfoB;
  QtNumberReadout* m_pReadoutBandB_VfoA;
  QtNumberReadout* m_pReadoutBandB_VfoB;
  QWidget* m_pBandARow;
  QWidget* m_pBandBRow;

};
