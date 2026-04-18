//
// Created by murray on 27/07/25.
//

#pragma once
#include "ConfigResult.h"
#include <ArduinoJson.h>

#ifdef USE_ETL_COLLECTIONS
#include <etl/string.h>
#else
#include <string>
#endif

#define TYPE_LENGTH_INCL_0 24

namespace Config
{
#ifdef USE_ETL_COLLECTIONS
  using TypeString = etl::string<TYPE_LENGTH_INCL_0>;
#else
  using TypeString = std::string;
#endif

  struct Alternative
  {
    TypeString type;
  };

  static Result fromJson(JsonVariantConst json, Alternative& fields)
  {
    if (json["type"]) {
      fields.type = json["type"].as<const char *>();
      return Result::OK;
    }
    return Result::MISSING_TYPE_FOR_CONFIG;
  }
}
