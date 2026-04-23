//
// Created by murray on 16/12/25.
//

#include "io/control/device/gpio/Gpio.h"
#include "io/control/device/gpio/GpioException.h"
#include "core/config-settings/settings/RadioSettings.h"
#include "GpioBandSelector.h"


GpioBandSelector::GpioBandSelector() :
  m_defaultOut(0),
  m_currentOut(0)
{
  // m_frequencySettingPath = RadioSettings::getSettingPath("tx.rf.frequency");
  // m_offsetSettingPath = RadioSettings::getSettingPath("tx.rf.offset");
}

ResultCode
GpioBandSelector::configure(const Config::DigitalOutput::BandSelector::Fields& config)
{
  ResultCode rc = DigitalOutput::configure(config);
  if (rc == ResultCode::OK) {
    m_defaultOut = config.defaultOut;
    m_bands = config.bands;
  }
  return rc;
}

void
GpioBandSelector::applySettings(const RadioSettings& settings)
{
  if (settings.hasSettingChanged(RadioSettings::BAND)) {
    const BandSettings* pBandSettings = settings.getFocusBandSettings();
    const RfSettings& rfSettings = pBandSettings->getTxRfSettings();
    if (rfSettings.hasSettingChanged( RfSettings::VFO)) {
      uint32_t frequency = rfSettings.getVfo();
      uint32_t output = getBandOutput(frequency);
      applyOutput(output);
    }
  }
}

void
GpioBandSelector::applySettingUpdate(SettingUpdate& setting)
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
GpioBandSelector::getBandOutput(uint32_t frequency) const
{
  // Lookup the output for the given frequency
  for ( auto& band : m_bands) {
    if (frequency >= band.fromFrequency && frequency <= band.toFrequency) {
      return band.outValue;
    }
  }
  return m_defaultOut;
}

void
GpioBandSelector::applyOutput(uint32_t output)
{
  m_currentOut = output;
  std::vector<bool> values(m_lines.size(), false);
  for (size_t i = 0; i < m_lines.size(); ++i) {
    values.at(i) = ((output >> i) & 0x1u) != 0;
  }
  m_pLines->setLineValues(m_lines, values);
}
