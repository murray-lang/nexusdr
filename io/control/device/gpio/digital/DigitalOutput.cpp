//
// Created by murray on 25/11/25.
//

#include "DigitalOutput.h"

#include "io/control/ControlException.h"
#include "io/control/device/gpio/Gpio.h"
#include "io/control/device/gpio/GpioException.h"
#include "settings/RadioSettings.h"

DigitalOutput::DigitalOutput() :
  GpioLines(Direction::OUTPUT)
{
}

void
DigitalOutput::configure(const ConfigBase* pConfig)
{
  auto* config = dynamic_cast<const DigitalOutputConfig*>(pConfig);
  GpioLines::configure(config);
  const std::string& strSettingPath = config->getSettingPath();
  m_settingPath = RadioSettings::getSettingPath(strSettingPath);
  setId(strSettingPath);
}

void
DigitalOutput::applySingleSetting(const SingleSetting& setting)
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

void
DigitalOutput::open()
{
  if (m_pLines != nullptr) {
    throw GpioException("DigitalOutput already open");
  }
  Gpio& gpio = Gpio::getInstance();
  DigitalOutputLinesRequest* pLines = gpio.requestDigitalOutputs("digitalOutputs", { this });
  m_pLines.reset(pLines);
}

void
DigitalOutput::close()
{
  if (m_pLines == nullptr) {
    throw GpioException("DigitalOutput not open");
  }
  m_pLines->release();
  m_pLines.reset();
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
