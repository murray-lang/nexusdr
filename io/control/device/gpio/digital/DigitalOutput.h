//
// Created by murray on 25/11/25.
//

#pragma once
#include "config/DigitalOutputConfig.h"
#include "io/control/ControlSink.h"
#include "io/control/device/gpio/GpioLines.h"
#include "settings/RadioSettingsSink.h"
#include "settings/SettingUpdatePath.h"


class DigitalOutputLinesRequest;

class DigitalOutput : public GpioLines, public ControlSink
{
public:
  DigitalOutput();
  ~DigitalOutput() override = default;

  // GpioLines overrides
  void configure(const ConfigBase* pConfig) override;
  bool discover() override;
  void open() override;
  void close() override;
  void exit() override;

  // PttSink override
  // Note: This DigitalOutput may well have a setting path corresponding to PTT, but it doesn't
  // respond to ptt() since this is informational. It already responds to the applySetting*() so
  // to respond here as well would be circular.
  void ptt(bool on) override {};

  void applySettingUpdate( SettingUpdate& setting) override;
  void applySettings(const RadioSettings& radioSettings, BandSettings* pBandSettings) override {}
  void applySettings(const RadioSettings& radioSettings) override {}

  void setValue(bool value);

protected:
  SettingUpdatePath m_settingPath;
  std::unique_ptr<DigitalOutputLinesRequest> m_pLines;

};
