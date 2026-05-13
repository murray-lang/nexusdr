#include "QtControlSourceConfig.h"

namespace Config::QtControlSource
{
  ResultCode fromJson(JsonVariantConst json, Fields& fields)
  {
    fields.type = type;
    return ResultCode::OK;
  }
}