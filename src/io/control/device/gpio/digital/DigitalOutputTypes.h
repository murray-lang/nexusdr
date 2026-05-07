#pragma once

#include "CrossPlatformTypes.h"
#include "DigitalOutput.h"
#include "GpioBandSelector.h"

using DigitalOutputVariant = variant<DigitalOutput, GpioBandSelector>;

#ifdef USE_ETL
// #include "etl/vector.h"
// #include "etl/variant.h"

// This is declared as a class to facilitate forward declarations
class DigitalOutputVariantVector : public etl::vector<DigitalOutputVariant, MAX_DIGITAL_OUTPUT_HANDLERS> {
public:
  using etl::vector<DigitalOutputVariant, MAX_DIGITAL_OUTPUT_HANDLERS>::vector;
};

using DigitalOutputValueVector = etl::vector<bool, MAX_GPIO_LINES_PER_DEVICE>;

#else
#include <vector>
#include <variant>

using DigitalOutputVariant = std::variant<DigitalOutput, GpioBandSelector>;
class DigitalOutputVariantVector : public std::vector<DigitalOutputVariant> {
public:
  using std::vector<DigitalOutputVariant>::vector;
};

using DigitalOutputValueVector = std::vector<bool>;

#endif