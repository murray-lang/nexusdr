#pragma once
#include "../base/ConfigBase.h"

#ifdef USE_ETL
#include <etl/string.h>
#else
#include <string>
#endif


namespace Config::Ui
{
  static constexpr auto type = "ui";

#ifdef USE_ETL
  using FaceString = etl::string<24>;
#else
  using FaceString = std::string;
#endif

  struct Fields
  {
    FaceString face;
  };

  extern ResultCode fromJson(JsonVariantConst json, Fields& fields);
}

// using UiConfig = Config::Ui::Fields;
