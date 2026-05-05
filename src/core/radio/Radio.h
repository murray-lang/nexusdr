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
  ~Radio() override;

  ResultCode configure(const Config::Radio::Fields& config) override;
  void start() override;
  void stop() override;

  ResultCode applySettings(const RadioSettings& settings) override;

  ResultCode applySettingUpdate(SettingUpdate& update) override;

  void ptt(bool on) override;

protected:
  void pttOn();
  void pttOff();

private:
  optional<IqReceiver> m_receiver;
  optional<IqTransmitter> m_transmitter;
  RadioControl m_control;
};
