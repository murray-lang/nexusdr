#include "FunCubeConfig.h"

namespace Config::FunCube
{
  Result fromJson(JsonVariantConst json, Fields& fields)
  {
    fields.type = type;
    return Result::OK;
  }
}