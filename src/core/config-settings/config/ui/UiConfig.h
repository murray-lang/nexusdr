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

  extern Result fromJson(JsonVariantConst json, Fields& fields);
}

using UiConfig = Config::Ui::Fields;
