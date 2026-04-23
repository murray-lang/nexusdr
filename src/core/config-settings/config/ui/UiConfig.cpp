#include "UiConfig.h"

namespace Config::Ui
{
  Result fromJson(JsonVariantConst json, Fields& fields)
  {
    if (json["face"]) {
      fields.face = json["face"].as<const char *>();
    } else {
      fields.face = "default";
    }
    return Result::OK;
  }
}