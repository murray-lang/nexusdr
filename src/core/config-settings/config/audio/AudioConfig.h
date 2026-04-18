//
// Created by murray on 27/07/25.
//

#pragma once
#include "../base/ConfigBase.h"
#include <cstdint>

#ifdef USE_ETL_COLLECTIONS
#include <etl/string.h>
#else
#include <string>
#endif

#define SHORT_STRING_LENGTH_INCL_0 8
#define LONG_STRING_LENGTH_INCL_0 16

namespace Config::Audio
{
  static constexpr auto type = "audio";

#ifdef USE_ETL_COLLECTIONS
  using ShortString = etl::string<TYPE_LENGTH_INCL_0>;
  using LongString = etl::string<LONG_STRING_LENGTH_INCL_0>;
#else
  using ShortString = std::string;
  using LongString = std::string;
#endif

  struct Fields : Alternative
  {
    bool isInput = false;
    bool isIq = false;
    ShortString soundApi; // "alsa", "pulse", "jack", "oss", "dummy"
    LongString searchExpression;
    uint32_t sampleRate = 0;
    uint32_t channelCount = 0;
    ShortString format; // "sint8", "sint16", "sint24", "sint32", "float32", "float64"
  };

  static Result fromJson(JsonVariantConst json, Fields& fields)
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
      fields.channelCount = std::min(json["channelCount"].as<uint32_t>(), 2u);
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

  static void toJson(const Fields& fields, JsonObject& json)
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
