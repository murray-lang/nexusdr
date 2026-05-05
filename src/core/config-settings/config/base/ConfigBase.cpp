#include "ConfigBase.h"


namespace Config
{
  ResultCode fromJson(JsonVariantConst json, Alternative& fields)
  {
    if (json["type"].is<JsonVariantConst>()) {
      fields.type = json["type"].as<const char *>();
      return ResultCode::OK;
    }
    return ResultCode::ERR_CONFIG_MISSING_TYPE;
  }
}