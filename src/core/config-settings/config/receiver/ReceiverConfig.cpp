#include "ReceiverConfig.h"

namespace Config::Receiver
{
  Result fromJson(JsonVariantConst json, Fields& fields)
  {
    return IqIo::fromJson(json, fields.iqIo);
  }
}