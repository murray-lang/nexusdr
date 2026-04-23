#include "RotaryEncoderConfig.h"

namespace Config::RotaryEncoder
{
  Result fromJson(JsonVariantConst json, Fields& fields)
  {
    Result result = DigitalInput::fromJson(json, fields);
    fields.type = type;
    return result;
  }
}