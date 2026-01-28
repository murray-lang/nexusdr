//
// Created by murray on 27/1/26.
//

#pragma once

#include "../FaceBase.h"
#include <memory>

#include "settings/RadioSettings.h"

class QtTimeSeriesChart;
class QtPanadapter;
class Radio;

namespace Ui { class StandardFace; }

class StandardFace : public FaceBase {
  Q_OBJECT
public:
  explicit StandardFace(QWidget* parent = nullptr);
  ~StandardFace() override;

  void initialise(RadioSettings* pRadioSettings) override;

  void setRadio(Radio* radio) override;
  void notifyRadioSettingsChanged() override;

  void handleReceiverIq(const vsdrcomplex* data, uint32_t length, uint32_t sampleRate) override;
  void handleReceiverAudio(const vsdrreal* data, uint32_t length) override;
  void handleTransmitterIq(const vsdrcomplex* data, uint32_t length, uint32_t sampleRate) override;
  void handleTransmitterAudio(const vsdrreal* data, uint32_t length) override;

private:
  std::unique_ptr<Ui::StandardFace> ui;
  Radio* m_pRadio;
  QtTimeSeriesChart* m_pTimeSeriesChart;
  QtPanadapter* m_pPanadapter;
  uint32_t m_reportedIqSampleRate;
};