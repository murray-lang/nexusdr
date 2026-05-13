//
// Created by murray on 6/08/25.
//
#include "ControlSinkFactory.h"

#ifdef USE_GPIO
#include "core/config-settings/config/control/BandSelectorConfig.h"
#include "device/gpio/digital/GpioBandSelector.h"
#include "device/gpio/digital/DigitalOutput.h"
#include "device/gpio/digital/DigitalOutputs.h"
#endif

#ifdef IS_QT
#include "qt/QtControlSink.h"
#endif

#include "core/config-settings/config/control/FunCubeConfig.h"
#include "device/FunCubeDongle/FunCubeDongle.h"


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

#ifdef IS_QT
  if (holds_alternative<Config::QtControlSink::Fields>(config)) {
    QtControlSink qtcs;
    result = qtcs.configure(get<Config::QtControlSink::Fields>(config));
    if (result == ResultCode::OK) {
      sink.emplace<QtControlSink>(move(qtcs));
    }
    return result;
  }
#endif
  return ResultCode::ERR_CONTROL_SINK;
}
