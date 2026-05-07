//
// Created by murray on 6/08/25.
//
#include "ControlSinkFactory.h"

#include "core/config-settings/config/control/BandSelectorConfig.h"
#include "core/config-settings/config/control/FunCubeConfig.h"
#include "device/FunCubeDongle/FunCubeDongle.h"
#include "device/gpio/digital/GpioBandSelector.h"
#include "device/gpio/digital/DigitalOutput.h"
#include "device/gpio/digital/DigitalOutputs.h"

ResultCode
ControlSinkFactory::create(const Config::Control::SinkConfigVariant& config, ControlSinkVariant& sink)
{
  ResultCode result = ResultCode::OK;
  if (holds_alternative<Config::FunCube::Fields>(config)) {
    FunCubeDongle funCube;
    result = funCube.configure(get<Config::FunCube::Fields>(config));
    if (result == ResultCode::OK) {
      sink.emplace<FunCubeDongle>(funCube);
    }
    return result;
  }
#ifdef USE_GPIO
  if (holds_alternative<Config::DigitalOutputs::Fields>(config)) {
    DigitalOutputs douts;
    result = douts.configure(get<Config::DigitalOutputs::Fields>(config));
    if (result == ResultCode::OK) {
      sink.emplace<DigitalOutputs>(move(douts));
    }
    return result;
  }
  // if (holds_alternative<Config::BandSelector::Fields>(config)) {
  //   GpioBandSelector bandSelector;
  //   result = bandSelector.configure(get<Config::BandSelector::Fields>(config));
  //   if (result == ResultCode::OK) {
  //     sink.emplace<GpioBandSelector>(move(bandSelector));
  //   }
  //   return result;
  // }
#endif
  return ResultCode::ERR_CONTROL_SINK;
}
