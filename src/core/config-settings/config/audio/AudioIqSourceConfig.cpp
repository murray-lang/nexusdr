#include "AudioIqSourceConfig.h"

namespace Config::AudioIqSource
{
  ResultCode fromJson(const JsonVariantConst& json, Fields& fields)
  {
    fields.type = type;
    if (json["audioInput"].is<JsonVariantConst>()) {
      return Audio::fromJson(json["audioInput"], fields.audioInput);
    }
    return ResultCode::ERR_CONFIG_IQ_AUDIO_INPUT_MISSING;
  }
}