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

  extern Result fromJson(JsonVariantConst json, Fields& fields);

  extern void toJson(const Fields& fields, JsonObject& json);
}

using AudioConfig = Config::Audio::Fields;
