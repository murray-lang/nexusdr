#include "IqIoConfig.h"

namespace Config::IqIo
{
  static Result sourceFactory(const TypedJson& json, IqSourceVariant& sourceVariant)
  {
    Result result = Result::OK;
    if (json.type == AudioSignalIqSource::type) {
      AudioSignalIqSource::Fields fields{};
      result = AudioSignalIqSource::fromJson(json.config, fields);
      if (result == Result::OK) {
        sourceVariant = fields;
      }
      return result;
    }
    return Result::UNKNOWN_TYPE;
  }

  static Result outputFactory(const TypedJson& json, AudioOutputVariant& outputVariant)
  {
    Result result = Result::OK;
    if (json.type == Audio::type) {
      Audio::Fields fields{};
      result = Audio::fromJson(json.config, fields);
      if (result == Result::OK) {
        outputVariant = fields;
      }
      return result;
    }
    return Result::UNKNOWN_TYPE;
  }

  Result fromJson(JsonVariantConst json, Fields& fields)
  {
    if (!json["iqSource"]) return Result::MISSING_IQ_SOURCE;
    if (!json["audioOutput"]) return Result::MISSING_AUDIO_OUTPUT;

    TypedJson taggedSourceJson;
    Result result = taggedSourceJson.fromJson(json["iqSource"]);
    if (result != Result::OK) return result;

    result = sourceFactory(taggedSourceJson, fields.iqSource);
    if (result != Result::OK) return result;

    TypedJson taggedOutputJson;
    result = taggedOutputJson.fromJson(json["audioOutput"]);
    if (result != Result::OK) return result;

    return outputFactory(taggedOutputJson, fields.audioOutput);
  }
}