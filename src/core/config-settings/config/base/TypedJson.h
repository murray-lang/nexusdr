#pragma once
#include <ArduinoJson.h>
#include "ResultCode.h"
#include <algorithm>

#ifdef USE_ETL
#include <etl/string.h>
#else
#include <string>
#endif

#define MAX_TYPE_LENGTH_INCL_0 24

namespace Config
{
#ifdef USE_ETL
using TypeString = etl::string<MAX_TYPE_LENGTH_INCL_0>;
#else
  using TypeString = std::string;
#endif

  class TypedJson
  {
  public:
    TypeString type;
    JsonDocument config;
    TypedJson() = default;
    TypedJson(const TypedJson& rhs) = default;
    TypedJson(const JsonVariantConst& json)
    {
      fromJson(json);
    }
    TypedJson& operator=(const TypedJson& rhs)
    {
      if (this != &rhs) {
        type = rhs.type;
        config = rhs.config;
      }
      return *this;
    }
    TypedJson& operator=(const JsonVariantConst& json)
    {
      fromJson(json);
      return *this;
    }

    ResultCode fromJson(JsonVariantConst json)
    {
      if (json["type"].is<JsonVariantConst>()) {
        type = json["type"].as<const char *>();
        // Convert to all lower case for consistency.
        // The config JSON can have any case for readability, but internally they should be all lower.
#ifdef USE_ETL
        etl::to_lower_case(type);
#else
        std::transform(type.begin(), type.end(), type.begin(),
                 [](unsigned char c){ return std::tolower(c); });
#endif
      } else {
        return ResultCode::ERR_CONFIG_MISSING_TYPE;
      }
      if (json["config"].is<JsonVariantConst>()) {
        config.set(json["config"]);
      } else {
        return ResultCode::ERR_CONFIG_MISSING_CONFIG_FOR_TYPE;
      }
      return ResultCode::OK;
    }

    void toJson(JsonObject& json) const
    {
      json["type"] = type;
      json["config"] = config;
    }
  };
}