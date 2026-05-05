//
// Created by murray on 25/11/25.
//

#include "CrossPlatformTypes.h"
#include "DigitalOutputTypes.h"
#include "DigitalOutput.h"

#include "io/control/device/gpio/Gpio.h"
#include "core/config-settings/settings/RadioSettings.h"
#include "core/config-settings/config/control/DigitalOutputConfig.h"



DigitalOutput::DigitalOutput() :
  GpioLines(Direction::OUTPUT)
{
}

ResultCode
DigitalOutput::configure(const Config::DigitalOutput::Fields& config)
{
  ResultCode rc = configureLines(config);
  if (rc != ResultCode::OK) return rc;
  const SettingPathString& strSettingPath = config.settingPath;
  return RadioSettings::getSettingUpdatePath(strSettingPath, m_settingPath);
}

ResultCode
DigitalOutput::applySettingUpdate(SettingUpdate& setting)
{
  if (setting.getPath() == m_settingPath) {
    bool value = get<bool>(setting.getValue());
    setValue(value);
    return ResultCode::OK;
  }
  return ResultCode::ERR_SETTING_PATH_NOT_MATCHED;
}

bool
DigitalOutput::discover()
{
  return Gpio::isPresent();
}

ResultCode
DigitalOutput::open()
{
  if (m_linesRequest) {
    return ResultCode::OK;
  }
  Gpio& gpio = Gpio::getInstance();
  DigitalOutputVariant thisAsVariant = move(*this);
  DigitalOutputVariantVector thisInVector(1, thisAsVariant);
  m_linesRequest.emplace();
  ResultCode rc = gpio.requestDigitalOutputs("digitalOutputs", thisInVector, *m_linesRequest);
  if (rc != ResultCode::OK) return rc;

  if (!m_linesRequest) {
    return ResultCode::ERR_DIGITAL_OUTPUT_LINES;
  }
  return ResultCode::OK;
}

void
DigitalOutput::close()
{
  if (m_linesRequest) {
    m_linesRequest->release();
    m_linesRequest.reset();
  }
}

void
DigitalOutput::exit()
{

}

void
DigitalOutput::setValue(bool value)
{
  m_linesRequest->setLineValue(m_lines.at(0), value);
}
