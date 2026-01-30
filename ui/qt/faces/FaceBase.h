//
// Created by murray on 27/1/26.
//

#pragma once
#include <QWidget>

#include "SampleTypes.h"
#include "settings/RadioSettingsSink.h"
#include "settings/base/SettingUpdateSource.h"


class Radio;

class FaceBase : public QWidget //, public SettingUpdateSource //, public SettingUpdateSink
{
  Q_OBJECT
public:
  using QWidget::QWidget;
  FaceBase(QWidget* pParent)
    : QWidget(pParent)
    , m_pRadio(nullptr)
    // , m_pRadioSettings(nullptr)
  {}
  ~FaceBase() override = default;

  virtual void initialise(RadioSettings* pRadioSettings)
  {
    // m_pRadioSettings = pRadioSettings;
  }

  virtual void handleRadioSettingsChanged(RadioSettings* pRadioSettings) {}

  void setNames(const std::string& name, const std::string& label) { m_name = name; m_label = label; }
  [[nodiscard]] const std::string& getName() const { return m_name; }
  [[nodiscard]] const std::string& getLabel() const { return m_label; }
  virtual void setRadio(Radio* pRadio) { m_pRadio = pRadio; }
  [[nodiscard]] Radio* getRadio() const { return m_pRadio; }

  virtual void handleReceiverIq(
    RadioSettings* pRadioSettings,
    const vsdrcomplex* data,
    uint32_t length,
    uint32_t sampleRate) = 0;
  virtual void handleReceiverAudio(const vsdrreal* data, uint32_t length) = 0;
  virtual void handleTransmitterIq(
    RadioSettings* pRadioSettings,
    const vsdrcomplex* data,
    uint32_t length,
    uint32_t sampleRate) = 0;
  virtual void handleTransmitterAudio(const vsdrreal* data, uint32_t length) = 0;


protected:
  std::string m_name;
  std::string m_label;
  // RadioSettings* m_pRadioSettings;
  Radio* m_pRadio;
};
