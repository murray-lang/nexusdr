#include "ReceiverConfig.h"

#include "ResultCode.h"

namespace Config::Receiver
{
  ResultCode fromJson(JsonVariantConst json, Fields& fields)
  {
    if (json["iqIo"].is<JsonVariantConst>()) {
      return IqIo::fromJson(json["iqIo"], fields.iqIo);
    }
    return ResultCode::ERR_CONFIG_RX_NO_IQ_IO;
  }
}
