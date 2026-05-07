#include "TransmitterConfig.h"

namespace Config::Transmitter
{
  ResultCode fromJson(JsonVariantConst json, Fields& fields)
  {
    if (json["iqIo"].is<JsonVariantConst>()) {
      return IqIo::fromJson(json["iqIo"], fields.iqIo);
    }
    return ResultCode::ERR_CONFIG_TX_NO_IQ_IO;
  }
}