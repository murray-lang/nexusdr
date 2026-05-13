#include "QtControlSinkConfig.h"

namespace Config::QtControlSink
{
  ResultCode fromJson(JsonVariantConst json, Fields& fields)
  {
    fields.type = type;
    return ResultCode::OK;
  }
}