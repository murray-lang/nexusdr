#pragma once

#include "../base/ConfigBase.h"
#include "GpioLinesConfig.h"

#ifdef USE_ETL_COLLECTIONS
#include <etl/string.h>
#else
#include <string>
#endif

namespace Config::DigitalOutput
{
  static constexpr auto type = "digitaloutput";

#ifdef USE_ETL_COLLECTIONS
  using SettingPathString = etl::string<MAX_SETTING_PATH_LENGTH>;
#else
  using SettingPathString = std::string;
#endif


  struct Fields : Alternative, GpioLines::Fields
  {
    SettingPathString settingPath;
  };

  extern Result fromJson(JsonVariantConst json, Fields& fields);
}

// using DigitalOutputConfig = Config::DigitalOutput::Fields;
