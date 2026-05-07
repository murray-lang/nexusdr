#include "DigitalInputConfig.h"

namespace Config::DigitalInput
{
  ResultCode fromJson(JsonVariantConst json, Fields& fields)
  {
    fields.type = type;
    ResultCode result = GpioLines::fromJson(json, fields);
    if (result != ResultCode::OK) return result;

    if (json["settingPath"].is<JsonVariantConst>()) {
      fields.settingPath = json["settingPath"].as<const char *>();
    } else {
      return ResultCode::ERR_CONFIG_MISSING_SETTING_PATH;
    }

    if (json["activeHigh"].is<JsonVariantConst>()) {
      fields.activeHigh = json["activeHigh"];
    } else {
      fields.activeHigh = true;
    }

    if (json["debounce"].is<JsonVariantConst>()) {
      fields.debounce = json["debounce"];
    } else {
      fields.debounce = false;
    }
    return ResultCode::OK;
  }
}