#include "TransmitterConfig.h"

namespace Config::Transmitter
{
  ResultCode fromJson(JsonVariantConst json, Fields& fields)
  {
    return IqIo::fromJson(json, fields.iqIo);
  }
}