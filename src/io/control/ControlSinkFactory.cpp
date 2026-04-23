//
// Created by murray on 6/08/25.
//
#include "ControlSinkFactory.h"

#include "core/config-settings/config/control/BandSelectorConfig.h"
#include "core/config-settings/config/control/FunCubeConfig.h"
#include "device/FunCubeDongle/FunCubeDongle.h"
#include "device/gpio/digital/GpioBandSelector.h"
#include "device/gpio/digital/DigitalOutput.h"

// #ifdef USE_ETL_COLLECTIONS
//   using etl::visit;
// #else
//   using std::visit;
// #endif

ResultCode
ControlSinkFactory::create(const Config::Control::SinkVariant& config, ControlSinkVariant& sink)
{
  ResultCode result = ResultCode::OK;
  if (std::holds_alternative<Config::FunCube::Fields>(config)) {
    FunCubeDongle funCube;
    result = funCube.configure(std::get<Config::FunCube::Fields>(config));
    if (result == ResultCode::OK) {
      sink.emplace<FunCubeDongle>(funCube);
    }
    return result;
  }
#ifdef USE_GPIO
  if (std::holds_alternative<Config::DigitalOutput::Fields>(config)) {
    DigitalOutput dout;
    result = dout.configure(std::get<Config::DigitalOutput::Fields>(config));
    if (result == ResultCode::OK) {
      sink.emplace<DigitalOutput>(std::move(dout));
    }
    return result;
  }
  if (std::holds_alternative<Config::DigitalOutput::BandSelector::Fields>(config)) {
    GpioBandSelector bandSelector;
    result = bandSelector.configure(std::get<Config::DigitalOutput::BandSelector::Fields>(config));
    if (result == ResultCode::OK) {
      sink.emplace<GpioBandSelector>(std::move(bandSelector));
    }
    return result;
  }
#endif
  return ResultCode::ERR_CONTROL_SINK;
}
