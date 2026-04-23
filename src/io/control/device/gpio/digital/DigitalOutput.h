//
// Created by murray on 25/11/25.
//

#pragma once
#include <memory>

#include "core/config-settings/config/control/DigitalOutputConfig.h"
#include "io/control/ControlSink.h"
#include "io/control/device/gpio/GpioLines.h"
#include "core/config-settings/settings/RadioSettingsSink.h"
#include "core/config-settings/settings/base/SettingUpdatePath.h"
#include "DigitalOutputLinesRequest.h"

// class DigitalOutputLinesRequest;

class DigitalOutput : public GpioLines, public ControlSink
{
public:
  DigitalOutput();
  ~DigitalOutput() override = default;

  DigitalOutput(DigitalOutput&&) = default;
  DigitalOutput& operator=(DigitalOutput&&) = default;

  virtual ResultCode configure(const Config::DigitalOutput::Fields& config);
  bool discover() override;
  ResultCode open() override;
  void close() override;
  void exit() override;

  // PttSink override
  // Note: This DigitalOutput may well have a setting path corresponding to PTT, but it doesn't
  // respond to ptt() since this is informational. It already responds to the applySetting*() so
  // to respond here as well would be circular.
  void ptt(bool on) override {};

  void applySettingUpdate( SettingUpdate& setting) override;
  void applySettings(const RadioSettings& radioSettings) override {}

  void setValue(bool value);

protected:
  SettingUpdatePath m_settingPath;
  std::unique_ptr<DigitalOutputLinesRequest> m_pLines;

};
