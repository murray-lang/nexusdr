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

  static Result fromJson(JsonVariantConst json, Fields& fields)
  {
    fields.type = type;
    Result result = GpioLines::fromJson(json, fields);
    if (result != Result::OK) return result;

    if (json["settingPath"]) {
      fields.settingPath = json["settingPath"].as<const char *>();
    } else {
      return Result::MISSING_SETTING_PATH;
    }

    if (json["activeHigh"]) {
      fields.activeHigh = json["activeHigh"];
    } else {
      fields.activeHigh = true;
    }

    if (json["debounce"]) {
      fields.debounce = json["debounce"];
    } else {
      fields.debounce = false;
    }
    return Result::OK;
  }
}
