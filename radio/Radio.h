//
// Created by murray on 18/08/25.
//

#pragma once
#include <vector>

#include <io/control/ControlSink.h>
#include <io/control/ControlSource.h>
#include "config/RadioConfig.h"
#include <io/control/RadioControl.h>
#include "receiver/IqReceiver.h"
#include <settings/RadioSettingsSink.h>

#include "../settings/ModeSettings.h"
#include "settings/RadioSettings.h"
#include "transmitter/IqTransmitter.h"

class Radio : public RadioSettingsSink, PttSink {

public:
  explicit Radio(QObject *pEventTarget = nullptr);
  ~Radio() override;

  void configure(const RadioConfig* pConfig);
  void start();
  void stop();

  void applySettings(const RadioSettings& settings) override;
  void applySingleSetting(const SingleSetting& settingDelta) override;

  void ptt(bool on) override;

protected:
  void pttOn();
  void pttOff();

protected:
  RadioSettings m_settings;
  IqReceiver* m_pReceiver;
  IqTransmitter* m_pTransmitter;
  RadioControl m_control;

  QObject* m_pEventTarget;
};

