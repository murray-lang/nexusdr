//
// Created by murray on 29/9/25.
//

#pragma once

#include "../base/ConfigBase.h"

#ifdef USE_ETL
#include <etl/vector.h>
#include <etl/string.h>
#else
#include <vector>
#include <string>
#endif

#define MAX_STRING_LENGTH_INCL_0 10

namespace Config::GpioLines
{
#ifdef USE_ETL
  using ConfigString = etl::string<MAX_STRING_LENGTH_INCL_0>;
  using LinesVector = etl::vector<uint32_t, MAX_GPIO_LINES_PER_DEVICE>;
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

  extern ResultCode fromJson(JsonVariantConst json, Fields& fields);
};

// using GpioLinesConfig = Config::GpioLines::Fields;

