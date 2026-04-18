#pragma once
#include "../base/ConfigBase.h"

#ifdef USE_ETL_COLLECTIONS
#include <etl/string.h>
#else
#include <string>
#endif


namespace Config::Ui
{
  static constexpr auto type = "ui";

#ifdef USE_ETL_COLLECTIONS
  using FaceString = etl::string<24>;
#else
  using FaceString = std::string;
#endif

  struct Fields
  {
    FaceString face;
  };

  static Result fromJson(JsonVariantConst json, Fields& fields)
  {
    if (json["face"]) {
      fields.face = json["face"].as<const char *>();
    } else {
      fields.face = "default";
    }
    return Result::OK;
  }
}
