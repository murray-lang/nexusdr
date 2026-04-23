//
// Created by murray on 15/9/25.
//

#pragma once

#include "../base/ConfigBase.h"
#include "GpioLinesConfig.h"

#ifdef USE_ETL_COLLECTIONS
#include <etl/string.h>
#else
#include <string>
#endif

namespace Config::DigitalInput
{
  static constexpr auto type = "digitalinput";

#ifdef USE_ETL_COLLECTIONS
  using SettingPathString = etl::string<MAX_SETTING_PATH_LENGTH>;
#else
  using SettingPathString = std::string;
#endif

  struct Fields : Alternative, GpioLines::Fields
  {
    bool activeHigh;
    bool debounce;
    SettingPathString settingPath;
  };

  extern Result fromJson(JsonVariantConst json, Fields& fields);
}

// using DigitalInputConfig = Config::DigitalInput::Fields;
