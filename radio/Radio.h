//
// Created by murray on 18/08/25.
//

#ifndef RADIO_H
#define RADIO_H
#include <vector>

#include "../io/control/device/DeviceControl.h"
#include "config/RadioConfig.h"
#include "receiver/IqReceiver.h"
#include "settings/sink/RadioSettingsSink.h"
#include "settings/sink/ReceiverSettingsSink.h"

class Radio : public RadioSettingsSink {

public:
  explicit Radio(QObject *eventTarget = nullptr);
  ~Radio() override;

  void configure(const RadioConfig& config);
  void start() const;
  void stop() const;

  void apply(const RadioSettings& settings) override;

protected:
  IqReceiver* m_pReceiver;
  // std::vector<DeviceControl*> m_deviceControllers;
};



#endif //RADIO_H
