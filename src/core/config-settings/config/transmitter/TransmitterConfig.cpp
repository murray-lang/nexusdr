#include "TransmitterConfig.h"

namespace Config::Transmitter
{
  Result fromJson(JsonVariantConst json, Fields& fields)
  {
    return IqIo::fromJson(json, fields.iqIo);
  }
}