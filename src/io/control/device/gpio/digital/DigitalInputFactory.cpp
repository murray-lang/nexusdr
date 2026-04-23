//
// Created by murray on 2025-09-04.
//

#include "DigitalInputFactory.h"

#include "DigitalInputs.h"
#include "core/util/StringUtils.h"

DigitalInput*
DigitalInputFactory::create(const  Config::DigitalInputs::DigitalInputVariant& config, DigitalInputVariant& input)
{
  ResultCode result = ResultCode::OK;
  if (std::holds_alternative<Config::DigitalInput::Fields>(config)) {
    DigitalInput di;
    result = di.configure(std::get<Config::DigitalInput::Fields>(config));
    if (result == ResultCode::OK) {
      input.emplace<DigitalInput>(di);
    }
    return result;
  }
  if (std::holds_alternative<Config::RotaryEncoder::Fields>(config)) {
    GpioRotaryEncoder encoder;
    result = encoder.configure(std::get<Config::RotaryEncoder::Fields>(config));
    if (result == ResultCode::OK) {
      input.emplace<GpioRotaryEncoder>(encoder);
    }
    return result;
  }
  return ResultCode::ERR_UNKNOWN_DIGITAL_INPUT;
}
