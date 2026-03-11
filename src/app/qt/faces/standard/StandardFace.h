//
// Created by murray on 27/1/26.
//

#pragma once

#include "../FaceBase.h"
#include <memory>

#include "core/config-settings/settings/RadioSettings.h"
#include "core/radio/Radio.h"

class QtFrequencyPanel;
class QtTimeSeriesChart;
class QtPanadapter;
class QtSMeter;
class Radio;

namespace Ui { class StandardFace; }

class StandardFace : public FaceBase {
  Q_OBJECT
public:
  explicit StandardFace(QWidget* parent = nullptr);
  ~StandardFace() override;

  void initialise(RadioSettings* pRadioSettings) override;

  // void connect(SettingUpdateSink* pSink) override { m_pExternalSettingsSink = pSink; }
  // void applySettingUpdate(SettingUpdate& settingDelta) override;

  void setRadio(Radio* radio) override;
  void handleRadioSettingsChanged(RadioSettings* pRadioSettings) override;

  void handleReceiverIq(
    RadioSettings* pRadioSettings,
    const vsdrcomplex* data,
    uint32_t length,
    uint32_t sampleRate) override;
  void handleReceiverAudio(const vsdrreal* data, uint32_t length) override;
  void handleReceiverMeter(float rssiDbFs, uint32_t sampleRate, std::optional<float> agcGainDb) override;

  void handleTransmitterIq(
    RadioSettings* pRadioSettings,
    const vsdrcomplex* data,
    uint32_t length,
    uint32_t sampleRate) override;
  void handleTransmitterAudio(const vsdrreal* data, uint32_t length) override;

protected:

  void updatePanadapter(BandSettings* bandSettings);
  void updateCursorA(RxPipelineSettings* rxPipelineSettings);
  void updateCursorB(RxPipelineSettings* rxPipelineSettings);

  // void notifySettingUpdate(SettingUpdate& settingUpdate) override
  // {
  //   if (m_pExternalSettingsSink != nullptr) {
  //     m_pExternalSettingsSink->applySettingUpdate(settingUpdate);
  //   }
  // }
  //------------------------------------------------------------------------------------------------------
  class InternalSettingUpdateSink : public SettingUpdateSink
  {
  public:
    InternalSettingUpdateSink(StandardFace* pOwningFace) : m_pOwningFace(pOwningFace) {}
    void applySettingUpdate(SettingUpdate& settingUpdate) override
    {
      if (m_pOwningFace->m_pRadio != nullptr) {
        m_pOwningFace->m_pRadio->applySettingUpdate(settingUpdate);
      }

    }
    void applySettingUpdates(SettingUpdate* updates, std::size_t count) override
    {
      if (m_pOwningFace->m_pRadio != nullptr) {
        m_pOwningFace->m_pRadio->applySettingUpdates(updates, count);
      }
    }
  protected:
    StandardFace* m_pOwningFace;
  };
  //-------------------------------------------------------------------------------------------------------
private:
  std::unique_ptr<Ui::StandardFace> ui;
  // SettingUpdateSink* m_pExternalSettingsSink;
  InternalSettingUpdateSink m_internalSettingsSink;
  QtTimeSeriesChart* m_pTimeSeriesChart;
  QtPanadapter* m_pPanadapter;
  QtFrequencyPanel* m_pFrequencyPanel;
  uint32_t m_reportedIqSampleRate;

  QtSMeter* m_pSmeter;
  float m_rssiMinDbFs;
  float m_rssiMaxDbFs;
};