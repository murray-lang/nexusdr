#include "AudioIqSourceConfig.h"

namespace Config::AudioIqSource
{
  Result fromJson(const JsonVariantConst& json, Fields& fields)
  {
    fields.type = type;
    return Audio::fromJson(json, fields.audioInput);
  }
}