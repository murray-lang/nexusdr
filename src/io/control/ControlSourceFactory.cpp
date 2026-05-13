#include "ControlSourceFactory.h"

#ifdef USE_GPIO
#include "core/config-settings/config/control/DigitalInputsConfig.h"
#include "device/gpio/digital/DigitalInputs.h"
#include "device/gpio/digital/GpioRotaryEncoder.h"
#endif

#ifdef IS_QT
#include "core/config-settings/config/control/QtControlSourceConfig.h"
#include "qt/QtControlSource.h"
#endif



ResultCode
ControlSourceFactory::create(const Config::Control::SourceConfigVariant& config, ControlSourceVariant& source)
{
  ResultCode result = ResultCode::OK;
#ifdef USE_GPIO
  if (holds_alternative<Config::DigitalInputs::Fields>(config)) {
    DigitalInputs dins;
    result = dins.configure(get<Config::DigitalInputs::Fields>(config));
    if (result == ResultCode::OK) {
      source.emplace<DigitalInputs>(move(dins));
    }
    return result;
  }
#endif
#ifdef IS_QT
  if (holds_alternative<Config::QtControlSource::Fields>(config)) {
    QtControlSource qtcs;
    result = qtcs.configure(get<Config::QtControlSource::Fields>(config));
    if (result == ResultCode::OK) {
      source.emplace<QtControlSource>(move(qtcs));
    }
    return result;
  }
#endif
  return ResultCode::ERR_CONTROL_NO_SOURCES_DEFINED;
}
