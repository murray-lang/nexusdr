//
// Created by murray on 23/3/26.
//

#pragma once
#include "CrossPlatformTypes.h"
#include "RadioBase.h"
#include "receiver/IqReceiver.h"
#include "transmitter/IqTransmitter.h"
#include "io/control/RadioControl.h"

// class RadioControl;
// class IqTransmitter;
// class IqReceiver;

class Radio : public RadioBase
{
public:
  Radio(EventTarget *pEventTarget = nullptr);
  ~Radio() override = default;

  ResultCode configure(const Config::Radio::Fields& config) override;
  ResultCode start(EventTarget* pEventTarget) override;
  void stop() override;

  ResultCode applySettings(const RadioSettings& settings) override;

  ResultCode applySettingUpdate(SettingUpdate& update) override;

  void markAllSettingsUnchanged();

  void ptt(bool on) override;

  ResultCode applySettingsToControlSinks();

protected:
  void pttOn();
  void pttOff();

private:
  optional<IqReceiver> m_receiver;
  optional<IqTransmitter> m_transmitter;
  RadioControl m_control;
};
