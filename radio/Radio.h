//
// Created by murray on 18/08/25.
//

#ifndef RADIO_H
#define RADIO_H
#include <vector>

#include "../io/control/ControlSink.h"
#include "../io/control/ControlSource.h"
#include "config/RadioConfig.h"
#include "../io/control/RadioControl.h"
#include "receiver/IqReceiver.h"
#include "settings/sink/PttSink.h"
#include "settings/sink/RadioSettingsSink.h"

class Radio : public RadioSettingsSink, PttSink {

public:
  explicit Radio(QObject *eventTarget = nullptr);
  ~Radio() override;

  void configure(const RadioConfig& config);
  void start();
  void stop();

  void apply(const RadioSettings& settings) override;

  void ptt(bool on) override;

protected:
  void pttOn();
  void pttOff();

protected:
  IqReceiver* m_pReceiver;
  RadioControl m_control;
};



#endif //RADIO_H
