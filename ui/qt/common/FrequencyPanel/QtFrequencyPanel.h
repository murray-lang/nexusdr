//
// Created by murray on 28/1/26.
//

#pragma once
#include <QObject>
#include <QWidget>
#include <memory>

#include "QtBandReadout.h"
#include "VfoReadout/QtVfoReadout.h"
#include "settings/bands/BandSelector.h"
#include "settings/bands/SplitBandId.h"
#include "settings/base/SettingUpdateSink.h"
#include "settings/base/SettingUpdateSource.h"

class RadioSettings;

namespace Ui { class QtFrequencyPanel; }

class QtFrequencyPanel : public QWidget, public SettingUpdateSource
{
  Q_OBJECT
public:
  explicit QtFrequencyPanel(QWidget* parent = nullptr);
  ~QtFrequencyPanel() override;

  void initialise(RadioSettings* pRadioSettings);
  void applyRadioSettings(RadioSettings* pRadioSettings, bool onlyIfChanged = true);

  void connectSettingUpdateSink(SettingUpdateSink* pSink) override
  {
    m_pSettingsSink = pSink;
    if (m_band1Readout) m_band1Readout->connectSettingUpdateSink(pSink);
    if (m_band2Readout) m_band2Readout->connectSettingUpdateSink(pSink);
  };

protected:
  void notifySettingUpdate(SettingUpdate& settingUpdate) override
  {
    if (m_pSettingsSink != nullptr) {
      m_pSettingsSink->applySettingUpdate(settingUpdate);
    }
  }

private slots:
  void onSplitRequested(SplitBandId whichBand);
  void onCloseRequested(SplitBandId whichBand);
  void onMultiVfoActionRequested(SplitBandId whichBand,
                            VfoId whichVfo,
                            MultiVfoAction action);
  void onVfoTxActionRequested(SplitBandId whichBand,
                            VfoId whichVfo,
                            VfoTxAction action);
  void onBandClicked(SplitBandId whichBand);
  void onVfoClicked(SplitBandId whichBand, VfoId whichVfo);
  void onTxBandClicked(SplitBandId whichBand);

private:
  void initialiseLayout();
  void applyFrequencyChanges(BandSelector& bandSelector, bool onlyIfChanged = true);
  void applyBandSelectorChange(BandSelector& bandSelector);

  void setPttProperty(bool ptt, bool repolish = true);
  void setIsSplitProperty(bool isSplit, bool repolish = true);

  void updateRowActionModes(bool hasBand1, bool hasBand2, bool isSplit);

private:
  std::unique_ptr<Ui::QtFrequencyPanel> ui;

  SettingUpdateSink* m_pSettingsSink;

  QtBandReadout* m_band1Readout;
  QtBandReadout* m_band2Readout;
};