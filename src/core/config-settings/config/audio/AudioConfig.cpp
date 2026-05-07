#include "AudioConfig.h"

namespace Config::Audio
{
  ResultCode fromJson(JsonVariantConst json, Fields& fields)
  {
    fields.type = type;
    if (json["soundApi"].is<JsonVariantConst>()) {
      fields.soundApi = json["soundApi"].as<const char*>();
    } else {
      return ResultCode::ERR_CONFIG_AUDIO_MISSING_API;
    }
    if (json["isInput"].is<JsonVariantConst>()) {
      fields.isInput = json["isInput"].as<bool>();
    } else {
      return ResultCode::ERR_CONFIG_AUDIO_MISSING_IS_INPUT;
    }
    if (json["isIq"].is<JsonVariantConst>()) {
      fields.isIq = json["isIq"].as<bool>();
    } else {
      return ResultCode::ERR_CONFIG_AUDIO_MISSING_IS_IQ;
    }
    if (json["searchExpression"].is<JsonVariantConst>()) {
      fields.searchExpression = json["searchExpression"].as<const char*>();
    } else {
      return ResultCode::ERR_CONFIG_AUDIO_MISSING_SEARCH_EXPRESSION;
    }
    if (json["sampleRate"].is<JsonVariantConst>()) {
      fields.sampleRate = json["sampleRate"].as<uint32_t>();
    } else {
      return ResultCode::ERR_CONFIG_AUDIO_MISSING_SAMPLE_RATE;
    }
    if (json["channelCount"].is<JsonVariantConst>()) {
      fields.channelCount = std::min(json["channelCount"].as<uint32_t>(), static_cast<uint32_t>(2));
    } else {
      return ResultCode::ERR_CONFIG_AUDIO_MISSING_CHANNELS;
    }
    if (json["format"].is<JsonVariantConst>()) {
      fields.format = json["format"].as<const char*>();
    } else {
      return ResultCode::ERR_CONFIG_AUDIO_MISSING_FORMAT;
    }
    return ResultCode::OK;
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