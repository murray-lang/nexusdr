#include "AudioConfig.h"

namespace Config::Audio
{
  Result fromJson(JsonVariantConst json, Fields& fields)
  {
    fields.type = type;
    if (json["soundApi"]) {
      fields.soundApi = json["soundApi"].as<const char*>();
    } else {
      return Result::AUDIO_MISSING_API;
    }
    if (json["isInput"]) {
      fields.isInput = json["isInput"].as<bool>();
    } else {
      return Result::AUDIO_MISSING_IS_INPUT;
    }
    if (json["isIq"]) {
      fields.isIq = json["isIq"].as<bool>();
    } else {
      return Result::AUDIO_MISSING_IS_IQ;
    }
    if (json["searchExpression"]) {
      fields.searchExpression = json["searchExpression"].as<const char*>();
    } else {
      return Result::AUDIO_MISSING_SEARCH_EXPRESSION;
    }
    if (json["sampleRate"]) {
      fields.sampleRate = json["sampleRate"].as<uint32_t>();
    } else {
      return Result::AUDIO_MISSING_SAMPLE_RATE;
    }
    if (json["channelCount"]) {
      fields.channelCount = std::min(json["channelCount"].as<uint32_t>(), static_cast<uint32_t>(2));
    } else {
      return Result::AUDIO_MISSING_CHANNELS;
    }
    if (json["format"]) {
      fields.format = json["format"].as<const char*>();
    } else {
      return Result::AUDIO_MISSING_FORMAT;
    }
    return Result::OK;
  }

  void toJson(const Fields& fields, JsonObject& json)
  {
    json["type"] = type;
    json["isInput"] = fields.isInput;
    json["isIq"] = fields.isIq;
    json["soundApi"] = fields.soundApi;
    json["searchExpression"] = fields.searchExpression;
    json["sampleRate"] = fields.sampleRate;
    json["channelCount"] = fields.channelCount;
    json["format"] = fields.format;
  }
}