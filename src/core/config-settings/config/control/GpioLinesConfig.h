//
// Created by murray on 29/9/25.
//

#pragma once

#include "../base/ConfigBase.h"

#ifdef USE_ETL_COLLECTIONS
#include <etl/vector.h>
#include <etl/string.h>
#else
#include <vector>
#include <string>
#endif

#define MAX_STRING_LENGTH_INCL_0 10
#define MAX_GPIO_LINES_PER_DEVICE 4

namespace Config::GpioLines
{
#ifdef USE_ETL_COLLECTIONS
  using ConfigString = etl::string<MAX_STRING_LENGTH_INCL_0>;
  using LinesVector = etl::vector<uint32_t, MAX_GPIO_LINES_PER_DEVICE>;
#else
  using ConfigString = std::string;
  using LinesVector = std::vector<uint32_t>;
#endif
  struct Fields
  {
    LinesVector lines;
    ConfigString direction; // "input" or "output"
    ConfigString bias; //"none", "pull-up" or "pull-down"
    ConfigString edge; //"rising", "falling" or "both"
  };

  extern Result fromJson(JsonVariantConst json, Fields& fields);
};

// using GpioLinesConfig = Config::GpioLines::Fields;

