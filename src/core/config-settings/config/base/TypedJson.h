#pragma once
#include <ArduinoJson.h>
#include "ConfigResult.h"
#include <algorithm>

#ifdef USE_ETL_COLLECTIONS
#include <etl/string.h>
#else
#include <string>
#endif

#define MAX_TYPE_LENGTH_INCL_0 24

namespace Config
{
#ifdef USE_ETL_COLLECTIONS
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

    Result fromJson(JsonVariantConst json)
    {
      if (json["type"]) {
        type = json["type"].as<const char *>();
        // Convert to all lower case for consistency.
        // The config JSON can have any case for readability, but internally they should be all lower.
        std::transform(type.begin(), type.end(), type.begin(),
                 [](unsigned char c){ return std::tolower(c); });
      } else {
        return Result::MISSING_TYPE_FOR_CONFIG;
      }
      if (json["config"]) {
        config.set(json["config"]);
      } else {
        return Result::MISSING_CONFIG_FOR_TYPE;
      }
      return Result::OK;
    }

    void toJson(JsonObject& json) const
    {
      json["type"] = type;
      json["config"] = config;
    }
  };
}