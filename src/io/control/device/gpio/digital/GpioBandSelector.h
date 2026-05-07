//
// Created by murray on 16/12/25.
//

#pragma once
#include "DigitalOutput.h"
#include "core/config-settings/config/control/BandSelectorConfig.h"

class GpioBandSelector : public DigitalOutput
{
public:
  GpioBandSelector();
  ~GpioBandSelector() override = default;

  GpioBandSelector(GpioBandSelector&&)  noexcept = default;
  GpioBandSelector& operator=(GpioBandSelector&&)  noexcept = default;

  ResultCode configure(const Config::BandSelector::Fields& config);

  ResultCode applySettings(const RadioSettings& settings) override;
  ResultCode applySettingUpdate(SettingUpdate& setting) override;
  void ptt(bool on) override {};

protected:
  [[nodiscard]] uint32_t getBandOutput(uint32_t frequency) const;

  ResultCode applyOutput(uint32_t output);
  // SettingPath m_frequencySettingPath;
  // SettingPath m_offsetSettingPath;
  uint32_t m_defaultOut;
  uint32_t m_currentOut;
  Config::BandSelector::BandsVector m_bands{};

};

