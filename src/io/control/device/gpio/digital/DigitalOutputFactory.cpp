#include "DigitalOutputFactory.h"

ResultCode
DigitalOutputFactory::create(const  Config::DigitalOutputs::DigitalOutputConfigVariant& config, DigitalOutputVariant& output)
{
  ResultCode result = ResultCode::OK;
  if (holds_alternative<Config::DigitalOutput::Fields>(config)) {
    DigitalOutput dout;
    result = dout.configure(get<Config::DigitalOutput::Fields>(config));
    if (result == ResultCode::OK) {
      output.emplace<DigitalOutput>(std::move(dout));
    }
    return result;
  }
  if (holds_alternative<Config::BandSelector::Fields>(config)) {
    GpioBandSelector selector;
    result = selector.configure(get<Config::BandSelector::Fields>(config));
    if (result == ResultCode::OK) {
      output.emplace<GpioBandSelector>(std::move(selector));
    }
    return result;
  }
  return ResultCode::ERR_UNKNOWN_DIGITAL_OUTPUT;
}