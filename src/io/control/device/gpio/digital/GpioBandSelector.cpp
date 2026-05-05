//
// Created by murray on 16/12/25.
//

#include "io/control/device/gpio/Gpio.h"
#include "core/config-settings/settings/RadioSettings.h"
#include "GpioBandSelector.h"

#include "DigitalOutputTypes.h"


GpioBandSelector::GpioBandSelector() :
  m_defaultOut(0),
  m_currentOut(0),
  m_bands{}
{
  // m_frequencySettingPath = RadioSettings::getSettingPath("tx.rf.frequency");
  // m_offsetSettingPath = RadioSettings::getSettingPath("tx.rf.offset");
}

ResultCode
GpioBandSelector::configure(const Config::BandSelector::Fields& config)
{
  ResultCode rc = DigitalOutput::configure(config);
  if (rc == ResultCode::OK) {
    m_defaultOut = config.defaultOut;
    m_bands = config.bands;
  }
  return rc;
}

ResultCode
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
  return ResultCode::OK;
}

ResultCode
GpioBandSelector::applySettingUpdate(SettingUpdate& setting)
{
  if (setting.getPath() == m_settingPath) {
    uint32_t frequency = get<uint32_t>(setting.getValue());
    uint32_t output = getBandOutput(frequency);
    if (output != m_currentOut) { 
      return applyOutput(output);
    }
    return ResultCode::OK;
  }
  return ResultCode::ERR_SETTING_PATH_NOT_MATCHED;
}

uint32_t
GpioBandSelector::getBandOutput(uint32_t frequency) const
{
  // Lookup the output for the given frequency
  for ( auto& band : m_bands) {
    if (band) {
      if (frequency >= band->fromFrequency && frequency <= band->toFrequency) {
        return band->outValue;
      }
    }

  }
  return m_defaultOut;
}

ResultCode
GpioBandSelector::applyOutput(uint32_t output)
{
  if (!m_linesRequest) {
    return ResultCode::ERR_DIGITAL_OUTPUT_NO_LINE_REQUEST;
  }
  m_currentOut = output;
  DigitalOutputVector values(m_lines.size(), false);
  for (size_t i = 0; i < m_lines.size(); ++i) {
    values.at(i) = ((output >> i) & 0x1u) != 0;
  }
  return m_linesRequest->setLineValues(m_lines, values);
}
