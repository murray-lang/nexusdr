//
// Created by murray on 18/08/25.
//

#ifndef RADIO_H
#define RADIO_H
#include <vector>

#include "../io/controller/SinkController.h"
#include "../io/controller/SourceController.h"
#include "config/RadioConfig.h"
#include "receiver/IqReceiver.h"
#include "settings/sink/PttSink.h"
#include "settings/sink/RadioSettingsSink.h"

class Radio : public RadioSettingsSink, PttSink {

public:
  explicit Radio(QObject *eventTarget = nullptr);
  ~Radio() override;

  void configure(const RadioConfig& config);
  void start() const;
  void stop() const;

  void apply(const RadioSettings& settings) override;

  void ptt(bool on) override;

protected:
  void pttOn();
  void pttOff();

protected:
  IqReceiver* m_pReceiver;
  std::vector<SinkController*> m_controlSinks;
  std::vector<SourceController*> m_controlSources;
};



#endif //RADIO_H
