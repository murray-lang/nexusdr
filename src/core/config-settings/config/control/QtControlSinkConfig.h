#pragma once

#include "../base/ConfigBase.h"

namespace Config::QtControlSink
{
  static constexpr auto type = "qtcontrolsink";

  struct Fields : Alternative
  {
  };

  extern ResultCode fromJson(JsonVariantConst json, Fields& fields);
}