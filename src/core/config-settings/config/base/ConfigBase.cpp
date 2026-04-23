#include "ConfigBase.h"


namespace Config
{
  Result fromJson(JsonVariantConst json, Alternative& fields)
  {
    if (json["type"]) {
      fields.type = json["type"].as<const char *>();
      return Result::OK;
    }
    return Result::MISSING_TYPE_FOR_CONFIG;
  }
}