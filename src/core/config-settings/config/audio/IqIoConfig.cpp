#include "IqIoConfig.h"

namespace Config::IqIo
{
  static ResultCode sourceFactory(const TypedJson& json, IqSourceConfigVariant& sourceVariant)
  {
    ResultCode result = ResultCode::OK;
    if (json.type == AudioSignalIqSource::type) {
      AudioSignalIqSource::Fields fields{};
      result = AudioSignalIqSource::fromJson(json.config, fields);
      if (result == ResultCode::OK) {
        sourceVariant = fields;
      }
      return result;
    }
    if (json.type == AudioIqSource::type) {
      AudioIqSource::Fields fields{};
      result = AudioIqSource::fromJson(json.config, fields);
      if (result == ResultCode::OK) {
        sourceVariant = fields;
      }
      return result;
    }
    return ResultCode::ERR_CONFIG_UNKNOWN_TYPE;
  }

  static ResultCode outputFactory(const TypedJson& json, AudioOutputConfigVariant& outputVariant)
  {
    ResultCode result = ResultCode::OK;
    if (json.type == Audio::type) {
      Audio::Fields fields{};
      result = Audio::fromJson(json.config, fields);
      if (result == ResultCode::OK) {
        outputVariant = fields;
      }
      return result;
    }
    return ResultCode::ERR_CONFIG_UNKNOWN_TYPE;
  }

  ResultCode fromJson(JsonVariantConst json, Fields& fields)
  {
    if (!json["iqSource"].is<JsonVariantConst>()) return ResultCode::ERR_CONFIG_MISSING_IQ_SOURCE;
    if (!json["audioOutput"].is<JsonVariantConst>()) return ResultCode::ERR_CONFIG_MISSING_AUDIO_OUTPUT;

    TypedJson taggedSourceJson;
    ResultCode result = taggedSourceJson.fromJson(json["iqSource"]);
    if (result != ResultCode::OK) return result;

    result = sourceFactory(taggedSourceJson, fields.iqSource);
    if (result != ResultCode::OK) return result;

    TypedJson taggedOutputJson;
    result = taggedOutputJson.fromJson(json["audioOutput"]);
    if (result != ResultCode::OK) return result;

    return outputFactory(taggedOutputJson, fields.audioOutput);
  }
}