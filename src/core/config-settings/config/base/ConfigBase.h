#pragma once
#ifdef USE_ETL_COLLECTIONS
#include <etl/string.h>
#else
#include <string>
#endif
#include "ConfigResult.h"
#include <ArduinoJson.h>

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

  extern Result fromJson(JsonVariantConst json, Alternative& fields);
}
