#include "UiConfig.h"

namespace Config::Ui
{
  ResultCode fromJson(JsonVariantConst json, Fields& fields)
  {
    if (json["face"].is<JsonVariantConst>()) {
      fields.face = json["face"].as<const char *>();
    } else {
      fields.face = "default";
    }
    return ResultCode::OK;
  }
}