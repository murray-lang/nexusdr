//
// Created by murray on 20/8/25.
//

#include "ControlSourceFactory.h"

#include "../../core/config-settings/config/control/DigitalInputsConfig.h"
#include "device/gpio/digital/DigitalInputs.h"
#include "device/gpio/digital/GpioRotaryEncoder.h"

ResultCode
ControlSourceFactory::create(const Config::Control::SourceConfigVariant& config, ControlSourceVariant& source)
{
#ifdef USE_GPIO
  ResultCode result = ResultCode::OK;

  if (holds_alternative<Config::DigitalInputs::Fields>(config)) {
    DigitalInputs dins;
    result = dins.configure(get<Config::DigitalInputs::Fields>(config));
    if (result == ResultCode::OK) {
      source.emplace<DigitalInputs>(move(dins));
    }
    return result;
  }
#endif
  return ResultCode::ERR_NO_CONTROL_SOURCES_DEFINED;
}
