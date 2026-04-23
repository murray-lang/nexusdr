#include "DigitalOutputConfig.h"

namespace Config::DigitalOutput
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
    return Result::OK;
  }
}