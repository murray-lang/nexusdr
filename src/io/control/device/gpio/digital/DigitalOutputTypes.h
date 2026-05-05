#pragma once

#include "CrossPlatformTypes.h"
#include "DigitalOutput.h"
#include "GpioBandSelector.h"

using DigitalOutputVariant = variant<DigitalOutput, GpioBandSelector>;

#ifdef USE_ETL
// #include "etl/vector.h"
// #include "etl/variant.h"

// This is declared as a class to facilitate forward declarations
class DigitalOutputVariantVector : public etl::vector<DigitalOutputVariant, MAX_DIGITAL_OUTPUTS> {
public:
  using etl::vector<DigitalOutputVariant, MAX_DIGITAL_OUTPUTS>::vector;
};

using DigitalOutputVector = etl::vector<bool, MAX_DIGITAL_OUTPUTS>;

#else
#include <vector>
#include <variant>

using DigitalOutputVariant = std::variant<DigitalOutput, GpioBandSelector>;
class DigitalOutputVariantVector : public std::vector<DigitalOutputVariant> {
public:
  using std::vector<DigitalOutputVariant>::vector;
};

using DigitalOutputVector = etl::vector<bool>;

#endif