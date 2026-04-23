#include "AudioSignalIqSourceConfig.h"

namespace Config::AudioSignalIqSource
{
  Result fromJson(const JsonVariantConst& json, Fields& fields)
  {
    Result result = AudioIqSource::fromJson(json, fields);
    fields.type = type; // Override what was set above
    if (result != Result::OK) return result;
    fields.reverse = json["reverse"] ? json["reverse"].as<bool>() : false;
    return Result::OK;
  }
}