#include "AudioSignalIqSourceConfig.h"

namespace Config::AudioSignalIqSource
{
  ResultCode fromJson(const JsonVariantConst& json, Fields& fields)
  {

    ResultCode result = AudioIqSource::fromJson(json, fields);
    fields.type = type; // Override what was set above
    if (result != ResultCode::OK) return result;
    fields.reverse = json["reverse"] ? json["reverse"].as<bool>() : false;
    return ResultCode::OK;
  }
}