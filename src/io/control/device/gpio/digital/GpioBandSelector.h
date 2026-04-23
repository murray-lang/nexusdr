//
// Created by murray on 16/12/25.
//

#pragma once
#include "DigitalOutput.h"
#include "core/config-settings/config/control/BandSelectorBandsConfig.h"
#include "core/config-settings/config/control/BandSelectorConfig.h"
#include "io/control/ControlSink.h"
#include "io/control/device/gpio/GpioLines.h"

class DigitalOutputLinesRequest;

class GpioBandSelector : public DigitalOutput
{
public:
  GpioBandSelector();
  ~GpioBandSelector() override = default;

  GpioBandSelector(GpioBandSelector&&) = default;
  GpioBandSelector& operator=(GpioBandSelector&&) = default;

  ResultCode configure(const Config::DigitalOutput::BandSelector::Fields& config);

  void applySettings(const RadioSettings& settings) override;
  void applySettingUpdate(SettingUpdate& setting) override;
  void ptt(bool on) override {};

protected:
  [[nodiscard]] uint32_t getBandOutput(uint32_t frequency) const;

  void applyOutput(uint32_t output);
  // SettingPath m_frequencySettingPath;
  // SettingPath m_offsetSettingPath;
  uint32_t m_defaultOut;
  std::vector<Config::DigitalOutput::BandSelector::Band::Fields> m_bands;
  uint32_t m_currentOut;

};

