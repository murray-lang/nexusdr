#include "DigitalInputFactory.h"


ResultCode
DigitalInputFactory::create(const Config::DigitalInputs::DigitalInputConfigVariant& config, DigitalInputVariant& input)
{
  ResultCode result = ResultCode::OK;
  if (holds_alternative<Config::DigitalInput::Fields>(config)) {
    DigitalInput di;
    result = di.configure(get<Config::DigitalInput::Fields>(config));
    if (result == ResultCode::OK) {
      input.emplace<DigitalInput>(std::move(di));
    }
    return result;
  }
  if (holds_alternative<Config::RotaryEncoder::Fields>(config)) {
    GpioRotaryEncoder encoder;
    result = encoder.configure(get<Config::RotaryEncoder::Fields>(config));
    if (result == ResultCode::OK) {
      input.emplace<GpioRotaryEncoder>(std::move(encoder));
    }
    return result;
  }
  return ResultCode::ERR_UNKNOWN_DIGITAL_INPUT;
}
