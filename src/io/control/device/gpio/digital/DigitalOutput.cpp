//
// Created by murray on 25/11/25.
//

#include "DigitalOutput.h"

#include "io/control/device/gpio/Gpio.h"
#include "core/config-settings/settings/RadioSettings.h"

DigitalOutput::DigitalOutput() :
  GpioLines(Direction::OUTPUT)
{
}

ResultCode
DigitalOutput::configure(const Config::DigitalOutput::Fields& config)
{
  ResultCode rc = GpioLines::configureLines(config);
  const std::string& strSettingPath = config.settingPath;
  m_settingPath = RadioSettings::getSettingUpdatePath(strSettingPath);
  return rc;
}

void
DigitalOutput::applySettingUpdate(SettingUpdate& setting)
{
  if (setting.getPath() == m_settingPath) {
    bool value = std::get<bool>(setting.getValue());
    setValue(value);
  }
}

bool
DigitalOutput::discover()
{
  return Gpio::isPresent();
}

ResultCode
DigitalOutput::open()
{
  if (m_pLines != nullptr) {
    return ResultCode::OK;
  }
  Gpio& gpio = Gpio::getInstance();
  DigitalOutputLinesRequest* pLines = gpio.requestDigitalOutputs("digitalOutputs", { this });
  m_pLines.reset(pLines);
  if (m_pLines == nullptr) {
    return ResultCode::ERR_DIGITAL_OUTPUT_LINES;
  }
  return ResultCode::OK;
}

void
DigitalOutput::close()
{
  if (m_pLines != nullptr) {
    m_pLines->release();
    m_pLines.reset();
  }
}

void
DigitalOutput::exit()
{

}

void
DigitalOutput::setValue(bool value)
{
  m_pLines->setLineValue(m_lines.at(0), value);
}
