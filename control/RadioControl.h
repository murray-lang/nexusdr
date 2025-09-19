//
// Created by murray on 2025-08-24.
//

#ifndef CUTESDR_VK6HL_RADIOCONTROL_H
#define CUTESDR_VK6HL_RADIOCONTROL_H
#include <vector>

#include "ControlSink.h"
#include "ControlSource.h"
#include <config/ControlConfig.h>


class RadioControl : public RadioSettingsSink, public PttSink
{
public:
  RadioControl() = default;
  ~RadioControl() override = default;

  void configure(const ControlConfig* pConfig);
  void start();
  void stop();

  void apply(const RadioSettings& settings) override;
  void ptt(bool on) override;

protected:
  std::vector<ControlSink*> m_sinks;
  std::vector<ControlSource*> m_sources;
};


#endif //CUTESDR_VK6HL_RADIOCONTROL_H