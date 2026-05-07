#pragma once
#ifdef USE_ETL
#include <etl/string.h>
#else
#include <string>
#endif
#include "ResultCode.h"
#include <ArduinoJson.h>

#define TYPE_LENGTH_INCL_0 24

namespace Config
{
#ifdef USE_ETL
  using TypeString = etl::string<TYPE_LENGTH_INCL_0>;
  using SettingPathString = etl::string<MAX_SETTING_PATH_LENGTH>;
#else
  using TypeString = std::string;
  using SettingPathString = std::string;
#endif

  struct Alternative
  {
    TypeString type;
  };

  extern ResultCode fromJson(JsonVariantConst json, Alternative& fields);
}
