#pragma once
#include "DigitalOutputTypes.h"
#include "DigitalOutput.h"
#include "core/config-settings/config/control/DigitalOutputsConfig.h"
#include "io/control/ControlSink.h"

class DigitalOutputs : public ControlSink
{
public:
  DigitalOutputs() = default;
  ~DigitalOutputs() override = default;

  DigitalOutputs(DigitalOutputs&&)  noexcept = default;
  DigitalOutputs& operator=(DigitalOutputs&&)  noexcept = default;

  // ControlBase overrides;
  ResultCode configure(const Config::DigitalOutputs::Fields& config);
  bool discover() override;
  ResultCode open() override;
  void close() override;
  void exit() override;

  ResultCode applySettings(const RadioSettings& settings) override;
  ResultCode applySettingUpdate(SettingUpdate& settingDelta) override;

  void ptt(bool on) override;

protected:
  ResultCode createOutputs(const Config::DigitalOutputs::Fields& config);

  DigitalOutputVariantVector m_outputs;
};
