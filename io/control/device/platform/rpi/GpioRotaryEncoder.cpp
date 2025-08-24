//
// Created by murray on 20/8/25.
//

#include "GpioRotaryEncoder.h"

GpioRotaryEncoder::GpioRotaryEncoder()
{

}

void
GpioRotaryEncoder::initialise(const nlohmann::json& json)
{
  if (json.contains("gpioPins")) {
  }
  if (json.contains("settingsPath")) {
    std::string strSettingsPath = json["settingsPath"];
    m_settingPath = RadioSettings::getSettingPath(strSettingsPath);
  }
}

bool
GpioRotaryEncoder::discover()
{
  return false;
}

void
GpioRotaryEncoder::open()
{

}

void
GpioRotaryEncoder::close()
{

}

void
GpioRotaryEncoder::exit()
{

}
