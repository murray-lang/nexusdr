#include "GpioLinesConfig.h"

namespace Config::GpioLines
{
  Result fromJson(JsonVariantConst json, Fields& fields)
  {
    fields.lines.clear();

    if (!json["lines"]) return Result::GPIO_MISSING_LINES;
    if (!json["direction"]) return Result::GPIO_MISSING_DIRECTION;

    for (JsonVariantConst line : json["lines"].as<JsonArrayConst>()) {
      fields.lines.push_back(line.as<uint32_t>());
    }
    fields.direction = json["direction"].as<const char *>();
    fields.bias = json["bias"] ? json["bias"].as<const char *>() : "none";
    fields.edge = json["edge"] ? json["edge"].as<const char *>() : "rising";
    return Result::OK;
  }
}