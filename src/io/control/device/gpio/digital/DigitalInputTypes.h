#pragma once

#include "CrossPlatformTypes.h"
#include "DigitalInput.h"
#include "GpioRotaryEncoder.h"

using DigitalInputVariant = variant<DigitalInput, GpioRotaryEncoder>;

#ifdef USE_ETL
#include "etl/vector.h"
#include "etl/unordered_map.h"


using DigitalInputMap = etl::unordered_map<uint32_t, reference_wrapper<DigitalInputVariant>, MAX_DIGITAL_INPUTS>;

// This is declared as a class to facilitate forward declarations
class DigitalInputVariantVector : public etl::vector<DigitalInputVariant, MAX_DIGITAL_INPUTS> {
public:
  using etl::vector<DigitalInputVariant, MAX_DIGITAL_INPUTS>::vector;
};
#else
#include <variant>

using DigitalInputMap = std::unordered_map<uint32_t, DigitalInputVariant>;

class DigitalInputVariantVector : public std::vector<DigitalInputVariant> {
public:
  using std::vector<DigitalInputVariant>::vector;
};

#endif

