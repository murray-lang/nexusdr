//
// Created by murray on 23/3/26.
//

#pragma once
#include "RadioBase.h"

class RadioControl;
class IqTransmitter;
class IqReceiver;

class RadioBackEnd : public RadioBase
{
public:
  RadioBackEnd(EventTarget *pEventTarget = nullptr);
  ~RadioBackEnd() override;

  void configure(const RadioConfig* pConfig) override;
  void start() override;
  void stop() override;

  void applySettings(const RadioSettings& settings) override;

  void applySettingUpdate(SettingUpdate& update) override;

  void ptt(bool on) override;

protected:
  void pttOn();
  void pttOff();

private:
  IqReceiver* m_pReceiver;
  IqTransmitter* m_pTransmitter;
  RadioControl* m_pControl;
};
