//
// Created by murray on 16/12/25.
//

#include "io/control/device/gpio/Gpio.h"
#include "io/control/device/gpio/GpioException.h"
#include "settings/RadioSettings.h"
#include "BandSelector.h"

#include "config/BandSelectorConfig.h"
#include "config/DigitalInputConfig.h"

BandSelector::BandSelector() :
  m_defaultOut(0),
  m_currentOut(0)
{
  // m_frequencySettingPath = RadioSettings::getSettingPath("tx.rf.frequency");
  // m_offsetSettingPath = RadioSettings::getSettingPath("tx.rf.offset");
}

void
BandSelector::configure(const ConfigBase* pConfig)
{
  DigitalOutput::configure(pConfig);
  auto* config = dynamic_cast<const BandSelectorConfig*>(pConfig);
  m_defaultOut = config->defaultOut;
  m_bands = config->bands;
}

void
BandSelector::applySettings(const RadioSettings& settings, BandSettings* pBandSettings)
{
  if (settings.hasSettingChanged(RadioSettings::PIPELINE)) {
    const RfSettings& rfSettings = pBandSettings->getTxRfSettings();
    if (rfSettings.hasSettingChanged(RfSettings::CENTRE_FREQUENCY | RfSettings::OFFSET)) {
      uint32_t frequency = rfSettings.getFrequency();
      frequency += rfSettings.getOffset();
      uint32_t output = getBandOutput(frequency);
      applyOutput(output);
    }
  }
}

void
BandSelector::applySettingUpdate(SettingUpdate& setting)
{
  if (setting.getPath() == m_settingPath) {
    uint32_t frequency = std::get<uint32_t>(setting.getValue());
    uint32_t output = getBandOutput(frequency);
    if (output != m_currentOut) { 
      applyOutput(output);
    }
  }
}

uint32_t
BandSelector::getBandOutput(uint32_t frequency) const
{
  // Lookup the output for the given frequency
  for ( auto& band : m_bands) {
    if (frequency >= band.fromFrequency && frequency <= band.toFrequency) {
      return band.out;
    }
  }
  return m_defaultOut;
}

void
BandSelector::applyOutput(uint32_t output)
{
  m_currentOut = output;
  std::vector<bool> values(m_lines.size(), false);
  for (size_t i = 0; i < m_lines.size(); ++i) {
    values.at(i) = ((output >> i) & 0x1u) != 0;
  }
  m_pLines->setLineValues(m_lines, values);
}
