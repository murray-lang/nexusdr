#pragma once
#include "../base/ConfigBase.h"

namespace Config::QtControlSource
{
  static constexpr auto type = "qtcontrolsource";

  struct Fields : Alternative
  {
  };

  extern ResultCode fromJson(JsonVariantConst json, Fields& fields);
}