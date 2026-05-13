//
// Created by murray on 27/1/26.
//

#pragma once
#include <QWidget>

#include "core/SampleTypes.h"
#include "core/config-settings/settings/RadioSettingsSink.h"
#include "core/config-settings/settings/base/SettingUpdateSource.h"
#include "core/radio/receiver/IqReceiverMetering.h"


class Radio;

class FaceBase : public QWidget //, public SettingUpdateSource //, public SettingUpdateSink
{
  Q_OBJECT
public:
  using QWidget::QWidget;
  FaceBase(QWidget* pParent)
    : QWidget(pParent)
    , m_pSettingUpdateSink(nullptr)
  {}
  ~FaceBase() override = default;

  virtual void initialise(RadioSettings* pRadioSettings)
  {
    // m_pRadioSettings = pRadioSettings;
  }

  virtual void handleRadioSettingsChanged(RadioSettings* pRadioSettings) {}

  void connectSettingsSink(SettingUpdateSink* pSink) { m_pSettingUpdateSink = pSink; }

  void setNames(const std::string& name, const std::string& label) { m_name = name; m_label = label; }
  [[nodiscard]] const std::string& getName() const { return m_name; }
  [[nodiscard]] const std::string& getLabel() const { return m_label; }
  // virtual void setRadio(Radio* pRadio) { m_pRadio = pRadio; }
  // [[nodiscard]] Radio* getRadio() const { return m_pRadio; }

  virtual void handleReceiverIq(
    RadioSettings* pRadioSettings,
    const ComplexSamplesMax* data,
    uint32_t length,
    uint32_t sampleRate) = 0;

  virtual void handleReceiverAudio(const RealSamplesMax* data, uint32_t length, uint32_t sampleRate) = 0;

  virtual void handleReceiverMeter(const IqReceiverMetering& metering) = 0;

  virtual void handleTransmitterIq(
    RadioSettings* pRadioSettings,
    const ComplexSamplesMax* data,
    uint32_t length,
    uint32_t sampleRate) = 0;
  virtual void handleTransmitterAudio(const RealSamplesMax* data, uint32_t length, uint32_t sampleRate) = 0;


protected:
  std::string m_name;
  std::string m_label;
  // RadioSettings* m_pRadioSettings;
  // Radio* m_pRadio;
  SettingUpdateSink* m_pSettingUpdateSink;
};
