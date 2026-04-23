#include "DigitalInputConfig.h"

namespace Config::DigitalInput
{
  Result fromJson(JsonVariantConst json, Fields& fields)
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