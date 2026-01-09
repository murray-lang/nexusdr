//
// Created by murray on 16/12/25.
//

#pragma once
#include "DigitalOutput.h"
#include "config/BandSelectorLookupItemConfig.h"
#include "io/control/ControlSink.h"
#include "io/control/device/gpio/GpioLines.h"

class DigitalOutputLinesRequest;

class BandSelector : public DigitalOutput
{
public:
  BandSelector();
  ~BandSelector() override = default;
  void configure(const ConfigBase* pConfig) override;

  void applySettings(const RadioSettings& settings, BandSettings* pBandSettings) override;
  void applySingleSetting(const SingleSetting& setting) override;
  void ptt(bool on) override {};

protected:
  [[nodiscard]] uint32_t getBandOutput(uint32_t frequency) const;

  void applyOutput(uint32_t output);
  // SettingPath m_frequencySettingPath;
  // SettingPath m_offsetSettingPath;
  uint32_t m_defaultOut;
  std::vector<BandSelectorLookupItemConfigFields> m_bands;
  uint32_t m_currentOut;

};

