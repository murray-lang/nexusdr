#include "FunCubeConfig.h"

namespace Config::FunCube
{
  ResultCode fromJson(JsonVariantConst json, Fields& fields)
  {
    fields.type = type;
    return ResultCode::OK;
  }
}