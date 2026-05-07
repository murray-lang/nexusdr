#include "RotaryEncoderConfig.h"

namespace Config::RotaryEncoder
{
  ResultCode fromJson(JsonVariantConst json, Fields& fields)
  {
    ResultCode result = DigitalInput::fromJson(json, fields);
    fields.type = type;
    return result;
  }
}