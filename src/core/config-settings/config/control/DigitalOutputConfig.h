#pragma once

#include "../base/ConfigBase.h"
#include "GpioLinesConfig.h"

#ifdef USE_ETL
#include <etl/string.h>
#else
#include <string>
#endif

namespace Config::DigitalOutput
{
  static constexpr auto type = "digitaloutput";

  struct Fields : Alternative, GpioLines::Fields
  {
    SettingPathString settingPath;
  };

  extern ResultCode fromJson(JsonVariantConst json, Fields& fields);
}

// using DigitalOutputConfig = Config::DigitalOutput::Fields;
